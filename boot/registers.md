# 레지스터 정리

## 1. 범용 레지스터
다양한 용도로 쓰이는 레지스터, 이름 뒤에 H/L 을 붙여 8비트씩 쪼개 쓰기도 함
- AX    /   Accumulator     /   산술 연산 및 BIOS 인터럽트 기능 번호 지정
- BX    /   Base            /   메모리 주소 지정 시 오프셋 저장
- CX    /   Count           /   반목문 횟수나 디스크 섹터 번호/개수 저장
- DX    /   Data            /   드라이브 번호(0x*0:HDD)나 입출력 포트 번호 저장

## 2. 세그먼트 레지스터
16비트의 한계를 넘어 메모리 주소를 찾기 위한 구역 정보
- CS    /   Code Segment    /   현재 실행 중인 코드가 들어있는 구역
- DS    /   Data Segment    /   프로그램에서 사용하는 데이터가 들어가있는 구역
- SS    /   Stack Segment/  /   함수 호출과 지역 변수에 쓰이는 스택이 들어있는 구역
- ES    /   Extra Segment/  /   int 0x13처럼 데이터를 추가로 로드할 목적지 주소 지정

## 3. 포인터 및 인덱스 레지스터
특정 위치를 가리키는 레지스터
- SI    /   Source Index    /   문자열 출력 시 원본 문자열의 주소를 가르킴
- SP    /   Stack Pointer   /   현재 스택의 가장 꼭대기(Top) 주소
- BP    /   Base Pointer    /   스택의 기준점(Bottom) 주소
- IP    /   Instruction Pointer / 다음에 실행할 명령어의 주소