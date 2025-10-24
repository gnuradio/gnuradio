# GNU Radio Linux Crypto Documentation

This directory contains the documentation for the GNU Radio Linux Crypto module.

## Documentation Structure

- `Doxyfile` - Doxygen configuration file for generating API documentation
- `README.md` - This file, containing documentation overview
- `api/` - Generated API documentation (created by Doxygen)
- `examples/` - Documentation examples and tutorials

## Building Documentation

### Prerequisites

Install Doxygen and related tools:

```bash
# Ubuntu/Debian
sudo apt-get install doxygen graphviz

# CentOS/RHEL/Fedora
sudo yum install doxygen graphviz
```

### Generating Documentation

1. **Generate API Documentation:**
   ```bash
   cd docs
   doxygen Doxyfile
   ```

2. **View Documentation:**
   ```bash
   # Open in web browser
   firefox html/index.html
   ```

## Documentation Contents

### API Documentation

The generated API documentation includes:

- **Class Documentation**: Complete class hierarchy and method descriptions
- **Function Documentation**: Detailed function signatures and parameters
- **Module Groups**: Organized by functionality (crypto, keyring, hardware)
- **Examples**: Code examples and usage patterns

### Key Documentation Sections

1. **Kernel Keyring Integration**
   - `kernel_keyring_source` class
   - Key management functions
   - Security considerations

2. **Linux Kernel Crypto API**
   - `kernel_crypto_aes` class
   - AF_ALG socket interface
   - Performance characteristics

3. **OpenSSL Integration**
   - `openssl_wrapper` class
   - Supported algorithms
   - Configuration options

4. **Hardware Security Modules**
   - `nitrokey_interface` class
   - Device management
   - Security features

## Documentation Standards

This documentation follows GNU Radio standards:

- **Doxygen Comments**: All C++ code includes comprehensive Doxygen documentation
- **Python Docstrings**: Python code follows PEP 257 docstring conventions
- **API Groups**: Related functionality grouped using `\ingroup` tags
- **Cross-References**: Extensive linking between related components

## Contributing to Documentation

When adding new features:

1. **Add Doxygen Comments**: Include `\brief`, `\param`, `\return` tags
2. **Update Examples**: Add usage examples for new functionality
3. **Update README**: Keep this file current with new features
4. **Test Documentation**: Ensure Doxygen generates without warnings

## Documentation Maintenance

- **Regular Updates**: Update documentation with each release
- **Version Control**: Keep documentation in sync with code changes
- **Review Process**: Include documentation review in code review process
- **User Feedback**: Incorporate user feedback to improve clarity

## External References

- [GNU Radio Documentation Standards](https://wiki.gnuradio.org/index.php/Documentation)
- [Doxygen Manual](https://www.doxygen.nl/manual/)
- [Python Documentation Standards](https://www.python.org/dev/peps/pep-0257/)
