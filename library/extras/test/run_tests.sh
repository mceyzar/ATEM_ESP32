#!/bin/bash

# ATEM ESP32 Library Test Runner
# Runs regression tests from the library structure

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Find project root (where this script is located)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo -e "${BLUE}ATEM ESP32 Library Test Runner${NC}"
echo -e "${BLUE}==============================${NC}"
echo "Project root: $PROJECT_ROOT"

# Check for Node.js
if ! command -v node >/dev/null 2>&1; then
    echo -e "${RED}Error: Node.js not found. Please install Node.js to run tests.${NC}"
    exit 1
fi

# Install dependencies if needed
if [[ ! -d "node_modules" ]] && [[ -f "$PROJECT_ROOT/devSupport/package.json" ]]; then
    echo -e "${YELLOW}Installing Node.js dependencies...${NC}"
    cp "$PROJECT_ROOT/devSupport/package.json" .
    npm install
fi

# Run the appropriate test based on arguments
case "${1:-regression}" in
    "unit")
        echo -e "${GREEN}Running unit tests...${NC}"
        cd "$PROJECT_ROOT/library/test"
        node library_test_suite.js --unit-only
        ;;
    "integration"|"hardware")
        echo -e "${GREEN}Running hardware integration tests...${NC}"
        cd "$PROJECT_ROOT/library/test"
        node library_test_suite.js --hardware
        ;;
    "regression"|*)
        echo -e "${GREEN}Running regression tests...${NC}"
        if [[ -f "regression_test.js" ]]; then
            AUTOMATED=true node regression_test.js
        else
            echo -e "${RED}Error: regression_test.js not found in current directory${NC}"
            echo "Current directory: $(pwd)"
            exit 1
        fi
        ;;
esac

echo -e "${GREEN}Test execution completed!${NC}"
