#!/bin/bash
# Master Test Runner for webserv
# Usage: chmod +x run_all_tests.sh && ./run_all_tests.sh

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

SERVER="http://localhost:8080"

print_banner() {
    echo -e "${CYAN}"
    echo "=================================================================="
    echo "                    WEBSERV TEST SUITE"
    echo "=================================================================="
    echo -e "${NC}"
}

check_server() {
    echo -e "${YELLOW}Checking if webserver is running on $SERVER...${NC}"
    if curl -s --connect-timeout 3 $SERVER > /dev/null 2>&1; then
        echo -e "${GREEN}✅ Server is running${NC}"
        return 0
    else
        echo -e "${RED}❌ Server is not responding${NC}"
        echo -e "${YELLOW}Please start the server with: ./webserv config/angela.conf${NC}"
        return 1
    fi
}

run_test_suite() {
    local test_file="$1"
    local test_name="$2"
    
    if [ -f "$test_file" ]; then
        echo -e "\n${PURPLE}Starting $test_name...${NC}"
        chmod +x "$test_file"
        if ./"$test_file"; then
            echo -e "${GREEN}✅ $test_name completed${NC}"
        else
            echo -e "${RED}❌ $test_name failed${NC}"
        fi
    else
        echo -e "${RED}❌ $test_file not found${NC}"
    fi
}

print_menu() {
    echo -e "\n${CYAN}Select test suite to run:${NC}"
    echo -e "${WHITE}1)${NC} GET method tests"
    echo -e "${WHITE}2)${NC} POST method tests" 
    echo -e "${WHITE}3)${NC} DELETE method tests"
    echo -e "${WHITE}4)${NC} Error handling & edge cases"
    echo -e "${WHITE}5)${NC} Run ALL tests"
    echo -e "${WHITE}6)${NC} Exit"
    echo -e "\n${YELLOW}Enter your choice (1-6):${NC} "
}

print_banner

# Check if server is running
if ! check_server; then
    exit 1
fi

# Make all test scripts executable
chmod +x test_get.sh test_post.sh test_delete.sh test_errors.sh 2>/dev/null

while true; do
    print_menu
    read -r choice
    
    case $choice in
        1)
            run_test_suite "test_get.sh" "GET Method Tests"
            ;;
        2)
            run_test_suite "test_post.sh" "POST Method Tests"
            ;;
        3)
            run_test_suite "test_delete.sh" "DELETE Method Tests"
            ;;
        4)
            run_test_suite "test_errors.sh" "Error Handling Tests"
            ;;
        5)
            echo -e "\n${PURPLE}🚀 Running ALL test suites...${NC}"
            run_test_suite "test_get.sh" "GET Method Tests"
            echo -e "\n${CYAN}Press Enter to continue to POST tests...${NC}"
            read -r
            run_test_suite "test_post.sh" "POST Method Tests"
            echo -e "\n${CYAN}Press Enter to continue to DELETE tests...${NC}"
            read -r
            run_test_suite "test_delete.sh" "DELETE Method Tests"
            echo -e "\n${CYAN}Press Enter to continue to Error tests...${NC}"
            read -r
            run_test_suite "test_errors.sh" "Error Handling Tests"
            echo -e "\n${GREEN}🎉 ALL TEST SUITES COMPLETED!${NC}"
            ;;
        6)
            echo -e "${GREEN}Goodbye!${NC}"
            exit 0
            ;;
        *)
            echo -e "${RED}Invalid choice. Please enter 1-6.${NC}"
            ;;
    esac
    
    echo -e "\n${CYAN}Press Enter to return to menu...${NC}"
    read -r
done