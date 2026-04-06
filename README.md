# 일단 대가리 박고 시작하는 커널 만들기

> 만들다보면 이해가 가겠지~

## 실행하는 법
```
make clean
make
make run
```

## 이상향
![alt text](docs/linux_kernel_diagram.png)
> 최대한 되는대로 다 구현해보는게 목표

## 개발 과정

- [x] **1. Booting & Basic Output**
  - Bootloader setup
  - Basic Video (VGA Text Mode)

- [x] **2. Low-Level Architecture**
  - I/O port communication (`inb`, `outb`, `io_wait`)
  - GDT (Global Descriptor Table) initialization

- [x] **3. Interrupt Management**
  - IDT (Interrupt Descriptor Table) setup
  - ISR (Interrupt Service Routines) & Assembly Wrappers
  - 8259A PIC (Programmable Interrupt Controller) Remapping