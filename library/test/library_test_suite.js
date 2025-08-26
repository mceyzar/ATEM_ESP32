#!/usr/bin/env node

/**
 * ATEM ESP32 Library Complete Test Suite
 * Runs all test types: Unit, Integration (Hardware), and Regression
 */

const { spawn } = require('child_process');
const path = require('path');
const fs = require('fs');

class LibraryTestSuite {
    constructor() {
        this.results = {
            unit: { status: 'pending', details: '' },
            integration: { status: 'pending', details: '' },
            regression: { status: 'pending', details: '' }
        };
        this.startTime = Date.now();
    }

    async runUnitTests() {
        console.log('🧪 Running Unit Tests (Mock-based, Fast)...\n');
        
        return new Promise((resolve) => {
            const proc = spawn('pio', ['test', '-e', 'native'], {
                cwd: __dirname,
                stdio: 'pipe'
            });

            let output = '';
            proc.stdout.on('data', (data) => {
                const text = data.toString();
                output += text;
                process.stdout.write(text);
            });

            proc.stderr.on('data', (data) => {
                const text = data.toString();
                output += text;
                process.stderr.write(text);
            });

            proc.on('close', (code) => {
                if (code === 0) {
                    this.results.unit.status = 'passed';
                    this.results.unit.details = 'All unit tests passed';
                } else {
                    this.results.unit.status = 'failed';
                    this.results.unit.details = 'Unit test failures detected';
                }
                resolve(code === 0);
            });

            proc.on('error', (err) => {
                this.results.unit.status = 'error';
                this.results.unit.details = `Error: ${err.message}`;
                resolve(false);
            });
        });
    }

    async runIntegrationTests() {
        console.log('\n🔌 Running Hardware Integration Tests...');
        console.log('NOTE: Requires ESP32 + ATEM hardware connected\n');
        
        return new Promise((resolve) => {
            const proc = spawn('pio', ['test', '-e', 'esp32_hardware'], {
                cwd: __dirname,
                stdio: 'pipe'
            });

            let output = '';
            proc.stdout.on('data', (data) => {
                const text = data.toString();
                output += text;
                process.stdout.write(text);
            });

            proc.stderr.on('data', (data) => {
                const text = data.toString();
                output += text;
                process.stderr.write(text);
            });

            proc.on('close', (code) => {
                if (code === 0) {
                    this.results.integration.status = 'passed';
                    this.results.integration.details = 'Hardware integration tests passed';
                } else {
                    this.results.integration.status = 'failed';
                    this.results.integration.details = 'Hardware integration test failures';
                }
                resolve(code === 0);
            });

            proc.on('error', (err) => {
                this.results.integration.status = 'error';
                this.results.integration.details = `Error: ${err.message}`;
                resolve(false);
            });
        });
    }

    async runRegressionTests() {
        console.log('\n🔄 Running Regression Tests (Full Protocol Validation)...');
        
        const regressionPath = path.join(__dirname, '..', 'extras', 'test', 'regression_test.js');
        
        if (!fs.existsSync(regressionPath)) {
            this.results.regression.status = 'skipped';
            this.results.regression.details = 'Regression test file not found';
            console.log('⚠️  Regression tests not available');
            return true;
        }

        return new Promise((resolve) => {
            const proc = spawn('node', [regressionPath], {
                stdio: 'pipe',
                env: { ...process.env, AUTOMATED: 'true' }
            });

            let output = '';
            proc.stdout.on('data', (data) => {
                const text = data.toString();
                output += text;
                process.stdout.write(text);
            });

            proc.stderr.on('data', (data) => {
                const text = data.toString();
                output += text;
                process.stderr.write(text);
            });

            proc.on('close', (code) => {
                if (code === 0) {
                    this.results.regression.status = 'passed';
                    this.results.regression.details = 'Full regression suite passed';
                } else {
                    this.results.regression.status = 'failed';
                    this.results.regression.details = 'Regression test failures detected';
                }
                resolve(code === 0);
            });

            proc.on('error', (err) => {
                this.results.regression.status = 'error';
                this.results.regression.details = `Error: ${err.message}`;
                resolve(false);
            });
        });
    }

    printResults() {
        const elapsed = (Date.now() - this.startTime) / 1000;
        
        console.log('\n' + '='.repeat(70));
        console.log('📊 ATEM ESP32 LIBRARY TEST RESULTS');
        console.log('='.repeat(70));
        
        const statusIcon = (status) => {
            switch(status) {
                case 'passed': return '✅';
                case 'failed': return '❌';
                case 'error': return '💥';
                case 'skipped': return '⏭️';
                default: return '⏳';
            }
        };

        console.log(`${statusIcon(this.results.unit.status)} Unit Tests: ${this.results.unit.status.toUpperCase()}`);
        console.log(`   ${this.results.unit.details}`);
        
        console.log(`${statusIcon(this.results.integration.status)} Integration Tests: ${this.results.integration.status.toUpperCase()}`);
        console.log(`   ${this.results.integration.details}`);
        
        console.log(`${statusIcon(this.results.regression.status)} Regression Tests: ${this.results.regression.status.toUpperCase()}`);
        console.log(`   ${this.results.regression.details}`);
        
        console.log('\n' + '-'.repeat(70));
        console.log(`Total execution time: ${elapsed.toFixed(1)}s`);
        
        // Only consider tests that were actually run
        const ranTests = Object.values(this.results).filter(
            result => result.status !== 'pending'
        );
        
        const allRanTestsPassed = ranTests.every(
            result => result.status === 'passed' || result.status === 'skipped'
        );
        
        const hasFailures = ranTests.some(
            result => result.status === 'failed' || result.status === 'error'
        );
        
        if (ranTests.length === 0) {
            console.log('\n⚠️  NO TESTS WERE RUN');
            return false;
        } else if (allRanTestsPassed && !hasFailures) {
            console.log('\n🎉 ALL RUN TESTS PASSED!');
            if (Object.values(this.results).some(result => result.status === 'pending')) {
                console.log('   (Some test categories were skipped)');
            }
            return true;
        } else {
            console.log('\n💥 SOME TESTS FAILED! Review failures before publishing.');
            return false;
        }
    }
}

async function main() {
    const args = process.argv.slice(2);
    
    if (args.includes('--help') || args.includes('-h')) {
        console.log('ATEM ESP32 Library Test Suite');
        console.log('');
        console.log('Usage: node library_test_suite.js [options]');
        console.log('');
        console.log('Options:');
        console.log('  --unit-only          Run only unit tests');
        console.log('  --no-hardware        Skip hardware integration tests');
        console.log('  --no-regression      Skip regression tests');
        console.log('  --help, -h           Show this help');
        console.log('');
        console.log('Test Types:');
        console.log('  Unit Tests           Fast mock-based tests (always run)');
        console.log('  Integration Tests    Hardware-dependent ESP32 + ATEM tests');
        console.log('  Regression Tests     Full protocol validation with real hardware');
        console.log('');
        console.log('Hardware tests require ESP32 device + ATEM switcher setup.');
        process.exit(0);
    }

    const unitOnly = args.includes('--unit-only');
    const noHardware = args.includes('--no-hardware');
    const noRegression = args.includes('--no-regression');

    const suite = new LibraryTestSuite();
    let success = true;

    try {
        // Always run unit tests (fast, no hardware required)
        success = await suite.runUnitTests();

        // Run hardware integration tests unless excluded
        if (!unitOnly && !noHardware && success) {
            success = await suite.runIntegrationTests() && success;
        } else if (!unitOnly && !noHardware && !success) {
            console.log('\n⚠️  Skipping hardware tests due to unit test failures');
        }

        // Run regression tests unless excluded
        if (!unitOnly && !noRegression && success) {
            success = await suite.runRegressionTests() && success;
        } else if (!unitOnly && !noRegression && !success) {
            console.log('\n⚠️  Skipping regression tests due to previous failures');
        }

    } catch (error) {
        console.error('Test suite execution error:', error);
        success = false;
    }

    const allPassed = suite.printResults();
    process.exit(allPassed ? 0 : 1);
}

if (require.main === module) {
    main();
}

module.exports = { LibraryTestSuite };
