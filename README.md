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
## 개발 과정

- [x] **1. Booting & Basic Output**
  - Bootloader setup
  - Basic Video (VGA Text Mode)

- [x] **2. Low-Level Architecture**
  - I/O port communication (`inb`, `outb`, `io_wait`)
  - GDT (Global Descriptor Table) initialization

- [x] **3. Interrupt Management**
  - IDT (Interrupt Descriptor Table) setup
  - ISR (Interrupt Service Routines)
  - 8259A PIC (Programmable Interrupt Controller) Remapping

- [ ] **4. Hardware Drivers**
  - PIT (Programmable Interval Timer) — 주기적 타이머 인터럽트
  - Keyboard Driver — 스캔코드 처리, 키맵 변환
  - Serial Port (UART) — 디버깅용 출력

- [ ] **5. Memory Management**
  - Physical Memory Manager — 페이지 프레임 할당
  - Paging — 가상 주소 공간, CR3/CR0 설정
  - Heap Allocator (`malloc` / `free`)

- [ ] **6. Standard Library**
  - `string.h` — `memcpy`, `memset`, `strcmp`, `strlen`
  - `stdlib.h` — 기본 유틸리티

- [ ] **7. Process Management**
  - TSS (Task State Segment) — 커널/유저 스택 전환
  - Task Structure — 프로세스 상태, 레지스터 저장
  - Context Switching — 태스크 전환 메커니즘
  - Scheduler — Round Robin 스케줄링

- [ ] **8. User Space**
  - System Call Interface — `int 0x80` 기반
  - Ring 3 전환 — 커널/유저 모드 분리
  - ELF Loader — 실행 파일 로딩

- [ ] **9. File System**
  - VFS (Virtual File System) — 추상화 레이어
  - FAT32 또는 ext2 구현

- [ ] **10. Shell**
  - 기본 명령어 파싱
  - 프로세스 실행