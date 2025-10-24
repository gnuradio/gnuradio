#!/bin/bash
# Build documentation for gr-linux-crypto

set -e

echo "Building GNU Radio Linux Crypto Documentation..."

# Check if Doxygen is installed
if ! command -v doxygen &> /dev/null; then
    echo "ERROR: Doxygen not found. Please install doxygen:"
    echo "  sudo apt-get install doxygen graphviz"
    exit 1
fi

# Check if Graphviz is installed (for diagrams)
if ! command -v dot &> /dev/null; then
    echo "WARNING: Graphviz not found. Diagrams will not be generated."
    echo "  Install with: sudo apt-get install graphviz"
fi

# Create output directory
mkdir -p html

# Generate documentation
echo "Generating API documentation with Doxygen..."
doxygen Doxyfile

# Check if documentation was generated
if [ -d "html" ] && [ -f "html/index.html" ]; then
    echo "Documentation generated successfully!"
    echo "Open html/index.html in your web browser to view the documentation."
else
    echo "ERROR: Documentation generation failed."
    exit 1
fi

# Generate documentation statistics
echo ""
echo "Documentation Statistics:"
echo "========================"
if [ -f "html/index.html" ]; then
    echo "API Documentation: Generated"
else
    echo "API Documentation: Failed"
fi

if [ -f "README.md" ]; then
    echo "README: Available"
else
    echo "README: Missing"
fi

if [ -f "examples.md" ]; then
    echo "Examples: Available"
else
    echo "Examples: Missing"
fi

if [ -f "architecture.md" ]; then
    echo "Architecture: Available"
else
    echo "Architecture: Missing"
fi

echo ""
echo "Documentation build completed successfully!"
echo "View the documentation at: html/index.html"
