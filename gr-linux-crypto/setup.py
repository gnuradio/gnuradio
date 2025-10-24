#!/usr/bin/env python3
"""
Setup script for gr-linux-crypto module.
"""

from setuptools import setup, find_packages
import os

# Read the README file
with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

# Read requirements
with open("requirements.txt", "r", encoding="utf-8") as fh:
    requirements = [line.strip() for line in fh if line.strip() and not line.startswith("#")]

setup(
    name="gr-linux-crypto",
    version="1.0.0",
    author="GNU Radio Linux Crypto Team",
    author_email="gr-linux-crypto@gnuradio.org",
    description="GNU Radio module for Linux kernel crypto infrastructure integration",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/gnuradio/gr-linux-crypto",
    packages=find_packages(),
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: GNU General Public License v3 (GPLv3)",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: C++",
        "Topic :: Communications",
        "Topic :: Security :: Cryptography",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    python_requires=">=3.8",
    install_requires=requirements,
    extras_require={
        "dev": [
            "pytest>=6.0",
            "pytest-cov>=2.0",
            "black>=21.0",
            "flake8>=3.8",
            "mypy>=0.800",
        ],
    },
    entry_points={
        "console_scripts": [
            "gr-linux-crypto-test=gr_linux_crypto.test:main",
        ],
    },
    include_package_data=True,
    package_data={
        "gr_linux_crypto": [
            "grc/*.yml",
            "examples/*.py",
        ],
    },
    zip_safe=False,
)

