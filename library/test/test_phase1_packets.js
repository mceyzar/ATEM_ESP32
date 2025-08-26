#!/usr/bin/env node

/**
 * Phase 1 Advanced Switching - Packet Validation Test
 * Validates ESP32 packet generation against known Sofie ATEM Connection packets
 */

const { spawn } = require('child_process');
const fs = require('fs');
const path = require('path');

// Known Sofie packet bytes from our capture session
const SOFIE_REFERENCE_PACKETS = {
    FadeToBlack: {
        name: 'Fade To Black Toggle (FtbA)',
        hex: '081880080000000000000001000c00004674624100000000',
        commandData: [0x00, 0x00, 0x00, 0x00], // ME 0, toggle
        description: 'Fade to black toggle for Mix Effect 0'
    },
    FadeToBlackRate: {
        name: 'Fade To Black Rate 25 frames (FtbC)',
        hex: '081880080000000000000002000c00004674624301001900',
        commandData: [0x01, 0x00, 0x19, 0x00], // mask=1, ME=0, rate=25, reserved=0
        description: 'Set fade to black rate to 25 frames for ME 0'
    },
    TransitionPosition: {
        name: 'Transition Position 5000 (CTPs)',
        hex: '081880080000000000000003000c00004354507300001388',
        commandData: [0x00, 0x00, 0x13, 0x88], // ME=0, reserved=0, position=5000 big-endian
        description: 'Set transition position to 5000 (50%) for ME 0'
    },
    PreviewTransition: {
        name: 'Preview Transition Enable (CTPr)',
        hex: '081880080000000000000004000c00004354507200010000',
        commandData: [0x00, 0x01, 0x00, 0x00], // ME=0, state=1, reserved=0,0
        description: 'Enable transition preview for ME 0'
    }
};

class Phase1PacketValidator {
    constructor() {
        this.results = {
            passed: 0,
            failed: 0,
            tests: []
        };
    }

    log(message) {
        console.log(`[PACKET-TEST] ${message}`);
    }

    hexStringToBytes(hexString) {
        const result = [];
        for (let i = 0; i < hexString.length; i += 2) {
            result.push(parseInt(hexString.substr(i, 2), 16));
        }
        return result;
    }

    extractCommandData(packetHex) {
        // ATEM packet structure:
        // [0-11]   Header (12 bytes)
        // [12-15]  Command header (4 bytes: length + reserved)
        // [16-19]  Command name (4 bytes: e.g., "FtbA")
        // [20-23]  Command data (4 bytes: the actual command parameters)
        
        const bytes = this.hexStringToBytes(packetHex);
        if (bytes.length >= 24) {
            return bytes.slice(20, 24); // Extract command data bytes
        }
        return null;
    }

    extractCommandName(packetHex) {
        const bytes = this.hexStringToBytes(packetHex);
        if (bytes.length >= 20) {
            return String.fromCharCode(bytes[16], bytes[17], bytes[18], bytes[19]);
        }
        return null;
    }

    validateCommandStructure(packetName, sofieHex, esp32Hex) {
        const test = {
            name: packetName,
            status: 'failed',
            details: []
        };

        try {
            // Extract command data from both packets
            const sofieData = this.extractCommandData(sofieHex);
            const esp32Data = this.extractCommandData(esp32Hex);
            
            // Extract command names
            const sofieCommand = this.extractCommandName(sofieHex);
            const esp32Command = this.extractCommandName(esp32Hex);

            // Validate command name
            if (sofieCommand === esp32Command) {
                test.details.push(`✅ Command name match: "${sofieCommand}"`);
            } else {
                test.details.push(`❌ Command name mismatch: Sofie="${sofieCommand}", ESP32="${esp32Command}"`);
                test.status = 'failed';
                return test;
            }

            // Validate command data length
            if (sofieData && esp32Data && sofieData.length === esp32Data.length) {
                test.details.push(`✅ Command data length: ${sofieData.length} bytes`);
            } else {
                test.details.push(`❌ Command data length mismatch: Sofie=${sofieData?.length}, ESP32=${esp32Data?.length}`);
                test.status = 'failed';
                return test;
            }

            // Validate command data bytes
            let dataMatches = true;
            for (let i = 0; i < sofieData.length; i++) {
                if (sofieData[i] === esp32Data[i]) {
                    test.details.push(`✅ Byte ${i}: 0x${sofieData[i].toString(16).padStart(2, '0')}`);
                } else {
                    test.details.push(`❌ Byte ${i}: Sofie=0x${sofieData[i].toString(16).padStart(2, '0')}, ESP32=0x${esp32Data[i].toString(16).padStart(2, '0')}`);
                    dataMatches = false;
                }
            }

            if (dataMatches) {
                test.status = 'passed';
                test.details.push('🎉 Perfect command data match!');
            } else {
                test.status = 'failed';
            }

        } catch (error) {
            test.details.push(`❌ Validation error: ${error.message}`);
            test.status = 'error';
        }

        return test;
    }

    async loadESP32Packets() {
        const logPath = path.join(__dirname, '..', 'logs', 'esp32_packets_captured.json');
        
        if (!fs.existsSync(logPath)) {
            throw new Error(`ESP32 packet capture file not found: ${logPath}`);
        }

        const data = JSON.parse(fs.readFileSync(logPath, 'utf8'));
        return data.packets || [];
    }

    mapESP32Packets(esp32Packets) {
        const mapped = {};
        
        for (const packet of esp32Packets) {
            const cmdName = this.extractCommandName(packet.hexString);
            
            switch (cmdName) {
                case 'FtbA':
                    mapped.FadeToBlack = packet.hexString;
                    break;
                case 'FtbC':
                    mapped.FadeToBlackRate = packet.hexString;
                    break;
                case 'CTPs':
                    mapped.TransitionPosition = packet.hexString;
                    break;
                case 'CTPr':
                    mapped.PreviewTransition = packet.hexString;
                    break;
            }
        }
        
        return mapped;
    }

    async runValidation() {
        this.log('🔍 Starting Phase 1 packet validation against Sofie reference...');
        
        try {
            // Load ESP32 captured packets
            const esp32Packets = await this.loadESP32Packets();
            const esp32Mapped = this.mapESP32Packets(esp32Packets);
            
            this.log(`📦 Loaded ${esp32Packets.length} ESP32 packets`);
            this.log(`🎯 Mapped ${Object.keys(esp32Mapped).length} Phase 1 commands`);

            // Validate each Phase 1 command
            for (const [key, reference] of Object.entries(SOFIE_REFERENCE_PACKETS)) {
                if (esp32Mapped[key]) {
                    this.log(`\n🧪 Testing ${reference.name}...`);
                    
                    const test = this.validateCommandStructure(
                        reference.name,
                        reference.hex,
                        esp32Mapped[key]
                    );
                    
                    this.results.tests.push(test);
                    
                    // Log test results
                    this.log(`   ${test.status === 'passed' ? '✅' : '❌'} ${test.name}`);
                    test.details.forEach(detail => this.log(`      ${detail}`));
                    
                    if (test.status === 'passed') {
                        this.results.passed++;
                    } else {
                        this.results.failed++;
                    }
                } else {
                    this.log(`❌ ESP32 packet not found for ${reference.name}`);
                    this.results.failed++;
                    this.results.tests.push({
                        name: reference.name,
                        status: 'missing',
                        details: ['ESP32 packet not captured']
                    });
                }
            }

        } catch (error) {
            this.log(`❌ Validation failed: ${error.message}`);
            this.results.failed++;
        }
    }

    generateReport() {
        const total = this.results.passed + this.results.failed;
        const passRate = total > 0 ? Math.round((this.results.passed / total) * 100) : 0;
        
        this.log('\n' + '='.repeat(60));
        this.log('📊 PHASE 1 PACKET VALIDATION REPORT');
        this.log('='.repeat(60));
        this.log(`Total Tests: ${total}`);
        this.log(`Passed: ${this.results.passed}`);
        this.log(`Failed: ${this.results.failed}`);
        this.log(`Pass Rate: ${passRate}%`);
        
        if (this.results.failed === 0) {
            this.log('\n🎉 ALL TESTS PASSED! ESP32 packets match Sofie reference perfectly!');
        } else {
            this.log('\n⚠️  Some tests failed. Check command data generation in ESP32 library.');
        }
        
        return this.results.failed === 0;
    }

    async saveReport() {
        const reportPath = path.join(__dirname, '..', 'logs', 'phase1_packet_validation_report.json');
        
        const report = {
            timestamp: new Date().toISOString(),
            summary: {
                total: this.results.passed + this.results.failed,
                passed: this.results.passed,
                failed: this.results.failed,
                passRate: Math.round((this.results.passed / (this.results.passed + this.results.failed)) * 100)
            },
            tests: this.results.tests,
            reference: SOFIE_REFERENCE_PACKETS
        };
        
        fs.writeFileSync(reportPath, JSON.stringify(report, null, 2));
        this.log(`💾 Report saved to: ${reportPath}`);
    }
}

async function main() {
    const validator = new Phase1PacketValidator();
    
    try {
        await validator.runValidation();
        const success = validator.generateReport();
        await validator.saveReport();
        
        process.exit(success ? 0 : 1);
        
    } catch (error) {
        console.error('❌ Validation failed:', error.message);
        process.exit(1);
    }
}

// Run validation if called directly
if (require.main === module) {
    main().catch(console.error);
}

module.exports = { Phase1PacketValidator, SOFIE_REFERENCE_PACKETS };
