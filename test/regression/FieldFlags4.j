
.class interface abstract FieldFlags4
.super java/lang/Object

.field public foo I

; No stack trace
; Expected Output:
; java.lang.ClassFormatError: (class: FieldFlags4, field: foo) Interface fields must have the public, static, and final flags set
