#!/usr/bin/env node

/**
 * ATEM ESP32 Library Test Runner
 * Runs unit tests and optionally hardware integration tests
 */

const { spawn } = require('child_process');
const path = require('path');

class TestRunner {
    constructor() {
        this.results = {
            unit: { passed: 0, failed: 0, total: 0 },
            integration: { passed: 0, failed: 0, total: 0 }
        };
    }

    async runUnitTests() {
        console.log('🧪 Running Unit Tests (Native)...\n');
        
        return new Promise((resolve, reject) => {
            const proc = spawn('pio', ['test', '-e', 'native'], {
                cwd: path.join(__dirname),
                stdio: 'inherit'
            });

            proc.on('close', (code) => {
                if (code === 0) {
                    console.log('\n✅ Unit tests passed!');
                    this.results.unit.passed = 1;
                } else {
                    console.log('\n❌ Unit tests failed!');
                    this.results.unit.failed = 1;
                }
                this.results.unit.total = 1;
                resolve(code === 0);
            });

            proc.on('error', (err) => {
                console.error('Error running unit tests:', err);
                reject(err);
            });
        });
    }

    async runIntegrationTests() {
        console.log('\n🔌 Running Hardware Integration Tests...');
        console.log('NOTE: Requires ESP32 connected with actual ATEM hardware\n');
        
        return new Promise((resolve, reject) => {
            const proc = spawn('pio', ['test', '-e', 'esp32_hardware'], {
                cwd: path.join(__dirname),
                stdio: 'inherit'
            });

            proc.on('close', (code) => {
                if (code === 0) {
                    console.log('\n✅ Integration tests passed!');
                    this.results.integration.passed = 1;
                } else {
                    console.log('\n❌ Integration tests failed!');
                    this.results.integration.failed = 1;
                }
                this.results.integration.total = 1;
                resolve(code === 0);
            });

            proc.on('error', (err) => {
                console.error('Error running integration tests:', err);
                reject(err);
            });
        });
    }

    printSummary() {
        console.log('\n' + '='.repeat(60));
        console.log('📊 TEST SUMMARY');
        console.log('='.repeat(60));
        
        const unitTotal = this.results.unit.total;
        const unitPassed = this.results.unit.passed;
        const integrationTotal = this.results.integration.total;
        const integrationPassed = this.results.integration.passed;
        
        console.log(`Unit Tests:        ${unitPassed}/${unitTotal} passed`);
        console.log(`Integration Tests: ${integrationPassed}/${integrationTotal} passed`);
        
        const totalPassed = unitPassed + integrationPassed;
        const totalTests = unitTotal + integrationTotal;
        
        console.log(`Total:             ${totalPassed}/${totalTests} passed`);
        
        if (totalPassed === totalTests && totalTests > 0) {
            console.log('\n🎉 All tests passed!');
            return true;
        } else {
            console.log('\n💥 Some tests failed!');
            return false;
        }
    }
}

async function main() {
    const args = process.argv.slice(2);
    const runHardware = args.includes('--hardware') || args.includes('-h');
    const unitOnly = args.includes('--unit-only') || args.includes('-u');
    
    if (args.includes('--help')) {
        console.log('ATEM ESP32 Library Test Runner');
        console.log('');
        console.log('Usage: node test_runner.js [options]');
        console.log('');
        console.log('Options:');
        console.log('  --unit-only, -u    Run only unit tests');
        console.log('  --hardware, -h      Run hardware integration tests');
        console.log('  --help              Show this help');
        console.log('');
        console.log('By default, runs unit tests only.');
        console.log('Hardware tests require ESP32 + ATEM hardware setup.');
        process.exit(0);
    }

    const runner = new TestRunner();
    let success = true;

    try {
        // Always run unit tests unless specifically excluded
        success = await runner.runUnitTests();

        // Run hardware tests if requested and unit tests passed
        if (!unitOnly && runHardware && success) {
            success = await runner.runIntegrationTests();
        } else if (runHardware && !success) {
            console.log('\n⚠️  Skipping hardware tests due to unit test failures');
        }

    } catch (error) {
        console.error('Test execution error:', error);
        success = false;
    }

    const allPassed = runner.printSummary();
    process.exit(allPassed ? 0 : 1);
}

if (require.main === module) {
    main();
}

module.exports = { TestRunner };
