# 일단 대가리 박고 시작하는 커널 만들기

> 만들다보면 이해가 가겠지~

## boot.asm 실행하는 법
```
nasm -f bin boot.asm -o boot.bin
qemu-system-x86_64 -drive format=raw,file=boot.bin -nographic
```