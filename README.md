# 일단 대가리 박고 시작하는 커널 만들기

> 만들다보면 이해가 가겠지~

## 실행하는 법
```
make clean
make
```

## 이상향
![alt text](docs/linux_kernel_diagram.png)
> 최대한 되는대로 다 구현해보는게 목표

## 개발 단계

### 1. feat/boot
- [x] "Hello BootLoader" 출력
- [x] 디스크에서 커널 읽어오기
- [x] 32비트 보호 모드 전환
- [x] C 커널과 연결

### 2. feat/video
- [ ] 출력 드라이버
- [ ] 출력 기능

### 3. feat/memory
- [ ] 물리 메모리 매니저
- [ ] 논리 메모리
- [ ] 페이징
- [ ] 가상 메모리