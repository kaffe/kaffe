
.class FieldFlags2
.super java/lang/Object

.field final volatile foo I

; No stack trace
; Expected Output:
; java.lang.ClassFormatError: (class: FieldFlags2, field: foo) Final and volatile cannot both be set
