# 개발 과정

> 직접 운영체제를 구현하며 부팅, 인터럽트, 메모리 관리,
> 실행 흐름 전환의 원리를 학습한다.
>
> 체크된 항목은 아래에 명시한 기본 기능을 구현했다는 의미이며,
> 범용 운영체제 수준의 완성도를 의미하지 않는다.

## [x] **1. Booting & Kernel Entry**
- BIOS 기반 부트로더 작성 및 커널 로딩
- Real Mode → 32-bit Protected Mode 전환
- GDT 구성 및 세그먼트 레지스터 설정
- 커널 스택 설정 및 C 커널 진입
>  옛날 사람들은 어셈블리어로 어떻게 코딩한거임?

## [x] **2. Interrupt & Exception Handling**
- I/O 포트 접근 함수 (`inb`, `outb`, `io_wait`)
- IDT 구성 및 ISR 진입 코드 작성
- 인터럽트 진입 시 레지스터 저장 및 복원
- CPU 예외 처리 및 디버깅 정보 출력
- 8259A PIC 리매핑 및 EOI 처리
> 와 개 어려운데?

## [x] **3. Hardware Drivers**
- PIT — 주기적 타이머 인터럽트
- Keyboard — 스캔코드 처리 및 문자 입력
- VGA — 문자 출력 및 화면 제어
- Serial (COM1 UART) — 폴링 기반 디버깅 출력
> 이게 무슨 개소리지..?

## [x] **4. Physical Memory Management**
- BIOS E820 메모리 맵 수집
- 사용 가능한 물리 메모리를 기준으로 PMM 초기화
- 비트맵 기반 4 KiB 페이지 프레임 관리
- `alloc_page` / `free_page` — 물리 페이지 할당 및 해제
- `reserve_region` — 커널·비트맵·힙·스택 영역 예약
- 페이지 해제 시 정렬·범위·중복 해제 검사
> ...

## [x] **5. Kernel Heap**
- 커널 힙 영역 초기화 및 PMM 예약
- `kmalloc` / `kfree` — 커널 동적 메모리 할당 및 해제
- 메모리 할당 후 값 저장 및 해제 동작 확인

## [x] **6. Paging & Address Translation**
- 4 KiB 페이지 기반 페이지 디렉터리·페이지 테이블 구성
- 초기 20 MiB Identity Mapping
- CR3 설정 및 CR0.PG 활성화
- `map_page` / `unmap_page` — 페이지 매핑 및 해제
- `invlpg` — 매핑 변경 후 해당 TLB 엔트리 무효화
- `translate_address` — 페이지 오프셋을 포함한 주소 변환
- `PAGE_USER` 매핑 시 PDE에도 사용자 접근 권한 반영
- NULL 페이지 매핑 해제 후 Page Fault 및 CR2 확인
- 두 가상 페이지가 같은 물리 페이지를 공유하는 동작 확인
- 한쪽 매핑 해제 후 다른 쪽 매핑이 유지되는 동작 확인

## [x] **7. Kernel Library & Formatted Output**
- `kstrlen` / `kstrcmp` — 문자열 길이 및 비교
- `kmemset` / `kmemcpy` — 메모리 초기화 및 복사
- `kmemmove` — 겹치는 메모리 영역의 복사
- `kprintf` / `kvsnprintf` — 형식 지정 출력 및 버퍼 크기 제한
- 시리얼 형식 지정 출력으로 디버깅 로그 통합
- 컴파일러 내장 기능을 이용한 가변 인자 처리

## [ ] **8. Kernel Threads**
- `context_switch` 어셈블리 코드 작성 및 검토
- 스레드 구조체와 스레드별 커널 스택 구성
- 최초 실행을 위한 초기 스택 구성
- 두 커널 스레드의 협력적 실행 전환 검증

## [ ] **9. Scheduling**
- 실행 가능한 스레드 및 상태 관리
- `yield` 기반 Round Robin 스케줄링
- PIT 인터럽트를 이용한 선점형 스케줄링

## [ ] **10. Kernel Shell**
- 한 줄 입력 및 기본 편집 처리
- 명령어·인자 파싱
- 커널 내부 명령 실행
- 메모리 및 스레드 상태 확인 명령

## [ ] **11. User Space & Processes**
- 프로세스별 가상 주소 공간 구성
- 사용자 코드·스택 매핑 및 접근 권한 분리
- TSS 설정 — 사용자 모드에서 커널 진입 시 사용할 스택 지정
- Ring 3 진입
- `int 0x80` 기반 시스템 콜 인터페이스
- 사용자 프로그램 로딩 및 프로세스 실행·종료
- ELF 실행 파일 로딩

## [ ] **12. Storage & File System**
- 블록 장치 읽기·쓰기
- 파일 시스템 구현 — FAT32 또는 ext2 등 검토
- 파일·디렉터리 접근 인터페이스
- 필요에 따라 VFS 도입
- 셸에서 파일 접근 및 사용자 프로그램 실행