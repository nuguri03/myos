# 일단 대가리 박고 시작하는 커널 만들기

> 만들다보면 이해가 가겠지~

## boot.asm 실행하는 법
```
nasm -f bin boot.asm -o boot.bin
qemu-system-x86_64 -drive format=raw,file=boot.bin -nographic
```

## feat/boot 개발 체크리스트
- [x] "Hello BootLoader" 출력
- [x] 디스크에서 커널 읽어오기
- [ ] 32비트 보호 모드 전환
- [ ] C 커널과 연결