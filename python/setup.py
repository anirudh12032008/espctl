from setuptools import setup, find_packages
from pathlib import Path
desc = (Path(__file__).parent / "README.md").read_text(encoding="utf-8")
setup(name="espctl", version="0.1.1", description="Control and mointor an ESP32 over serial", long_description=desc, long_description_content_type="text/markdown", packages=find_packages(), install_requires=["pyserial", "pandas", "matplotlib"], python_requires=">=3.8" )
