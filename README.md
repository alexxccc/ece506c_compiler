# Compiler Project – Initial Definition

## 1. Student Information
- **Names: Jimmy Payan, Alex Caulin-Cardo**
- **Program: ECE 506C**
- **Course: Compiler Design** 
- **Semester / Year: Spring 2026**

## 2. Project Overview
Provide a brief description of your compiler project (1–2 paragraphs).  
What problem does it solve? What kind of language are you designing or compiling?
Include the Programming language you will use to implement the compiler (e.g., C, C++, Java, Python, Rust, etc.)

This project implements a custom compiler for Arduino devices, specifically targeting the Arduino UNO (ATmega328P). The compiler translates a restricted subset of Arduino-style C++ sketches into AVR assembly code suitable for execution on the microcontroller. The goal is to provide an alternative compiler front-end to the standard Arduino toolchain, enabling greater control over parsing, semantic analysis, and code generation while maintaining compatibility with Arduino hardware.
The compiler will be implemented in Python and will include standard compiler phases such as lexical analysis, parsing, semantic checking, intermediate representation generation, and final code generation. Python is well-suited for rapid development and experimentation in a compiler course setting, and the relatively small scale of embedded programs makes performance constraints manageable. An intermediate “cleaned” C-like representation will be used to simplify translation from the source language to AVR assembly.

## 3. Source Language
- **Name of the source code language: Arduino sketch**

## 4. Target / Output Language
- **Output of the compiler: AVR assembly**  
  (e.g., LLVM IR, Assembly, C code, Java bytecode, custom VM code, etc.)
- **Reason for choosing this output language: Compatible with Arduino microcontrollers**

## 5. Compiler Features (Planned)
- Lexical analysis
- Syntax analysis using a generated parser (e.g., LL or LALR, depending on the chosen framework)
- Semantic analysis (type checking, scope resolution, and basic semantic rules)
- Intermediate representations: A simplified C-like intermediate form used to bridge high-level constructs and low-level code generation
- Code generation to AVR assembly

## 6. Tools and Libraries
List any tools, frameworks, or libraries you plan to use: 
- We will evaluate multiple Python-based parsing frameworks, including ANTLR, Lark, and PLY, before selecting a final parser generator. 
- Version control and project management will be handled using Git and GitHub.
- Testing will be performed using custom Arduino sketch test cases, with potential use of Python testing frameworks such as unittest or pytest.

## 7. Repository Information
- **Repository hosting service: GitHub**  
- **Repository URL: https://github.com/alexxccc/ece506c_compiler.git**  
Should be public 
## 8. Project Status
- [Yes ] Repository created
- [Yes ] README file added
- [In Progress] Initial project structure committed

## 9. AI Disclosure
- Generative AI was used in this project primarily for code generation.
- After generating the beginning of a section (ex: Bison file), AI was used to continue according to the grammar and guidelines set up manually.
- Concepts were understood before generating large sections of code

## 10. Additional Notes
Any additional comments, assumptions, or ideas related to the project.
