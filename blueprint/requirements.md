Functional Requirements
- [ ] CPU: responsible for defining instructions, addressing modes, communicating with registers, status flags and handling interrupts
- [ ] PPU: responsible for managing registers, pattern table, name table, sprites, rendering pipeline, and scrolling 
- [ ] APU: responsible for handling APU registers, generating squares or triangle waves
- [ ] Controller: controller register
- [ ] Mappers: responsible for memory mapping 

Non-Functional Requirements 
- [ ] Cross-platform support for different OS and major compilers  
- [ ] Modern C++ (C++17)
- [ ] Clang linting 

Language Specific Decisions
- [ ] Use of `std::variant` over inheritance 
- [ ] Use of `std::unique_ptr` for owning `ptr`
- [ ] Use raw `ptr` or `std::weak_ptr` for non-owning ptr
