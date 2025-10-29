<!--
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-29 14:40:06
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-10-29 14:54:32
 * @Copyright: MIT License
 * @Description: 
-->
# Screen

## Video Memory

The video memory is **32KB** in total and occupies the memory space with the address B8000H-BFFFFH.

A total of **8 pages** can be cached for each **4KB page**.

By default, **page 0**, which is `B8000H-B8FFFH`, is displayed onscreen immediately when written to this memory.

## Attribute byte format

| 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|---|---|---|---|---|---|---|---|
| BL | R | G | B | I | R | G | B |

- **BL**: Blinking
- **R**: Red
- **G**: Green
- **B**: Blue
- **I**: Intensity
- **R**: Red
- **G**: Green
- **B**: Blue

Attribute bytes can be set bitwise to match different foreground and background colors.

## Full ASCII code list

｜ HEX | Symbol | Description    |
|------|--------|----------------|
|  00  |  NUL   | Null character |
|  01  |  SOH   | Start of header|
|  02  |  STX   | Start of text  |
|  03  |  ETX   | End of text    |
|  04  |  EOT   | End of transmission|
|  05  |  ENQ   | Enquiry        |
|  06  |  ACK   | Acknowledge    |
|  07  |  BEL   | Bell           |
|  08  |  BS    | Backspace      |
|  09  |  HT    | Horizontal tab |
|  0A  |  LF    | Line feed      |
|  0B  |  VT    | Vertical tab   |
|  0C  |  FF    | Form feed      |
|  0D  |  CR    | Carriage return|
|  0E  |  SO    | Shift out      |
|  0F  |  SI    | Shift in       |
|  10  |  DLE   | Data link escape|
|  11  |  DC1   | Device control 1|
|  12  |  DC2   | Device control 2|
|  13  |  DC3   | Device control 3|
|  14  |  DC4   | Device control 4|
|  15  |  NAK   | Negative acknowledge|
|  16  |  SYN   | Synchronous idle|
|  17  |  ETB   | End of transmission block|
|  18  |  CAN   | Cancel         |
|  19  |  EM    | End of medium  |
|  1A  |  SUB   | Substitute     |
|  1B  |  ESC   | Escape         |
|  1C  |  FS    | File separator |
|  1D  |  GS    | Group separator|
|  1E  |  RS    | Record separator|
|  1F  |  US    | Unit separator |
|  20  |  SP    | Space          |
|  21  |  !     | Exclamation mark|
|  22  |  "     | Double quote   |
|  23  |  #     | Number sign    |
|  24  |  $     | Dollar sign    |
|  25  |  %     | Percent sign   |
|  26  |  &     | Ampersand      |
|  27  |  '     | Single quote   |
|  28  |  (     | Left parenthesis|
|  29  |  )     | Right parenthesis|
|  2A  |  *     | Asterisk       |
|  2B  |  +     | Plus sign      |
|  2C  |  ,     | Comma          |
|  2D  |  -     | Minus sign     |
|  2E  |  .     | Full stop      |
|  2F  |  /     | Slash          |
|  30  |  0     | Digit zero     |
|  31  |  1     | Digit one      |
|  32  |  2     | Digit two      |
|  33  |  3     | Digit three    |
|  34  |  4     | Digit four     |
|  35  |  5     | Digit five     |
|  36  |  6     | Digit six      |
|  37  |  7     | Digit seven    |
|  38  |  8     | Digit eight    |
|  39  |  9     | Digit nine     |
|  3A  |  :     | Colon          |
|  3B  |  ;     | Semicolon      |
|  3C  |  <     | Less-than sign |
|  3D  |  =     | Equals sign    |
|  3E  |  >     | Greater-than sign|
|  3F  |  ?     | Question mark  |
|  40  |  @     | At sign        |
|  41  |  A     | Uppercase A    |
|  42  |  B     | Uppercase B    |
|  43  |  C     | Uppercase C    |
|  44  |  D     | Uppercase D    |
|  45  |  E     | Uppercase E    |
|  46  |  F     | Uppercase F    |
|  47  |  G     | Uppercase G    |
|  48  |  H     | Uppercase H    |
|  49  |  I     | Uppercase I    |
|  4A  |  J     | Uppercase J    |
|  4B  |  K     | Uppercase K    |
|  4C  |  L     | Uppercase L    |
|  4D  |  M     | Uppercase M    |
|  4E  |  N     | Uppercase N    |
|  4F  |  O     | Uppercase O    |
|  50  |  P     | Uppercase P    |
|  51  |  Q     | Uppercase Q    |
|  52  |  R     | Uppercase R    |
|  53  |  S     | Uppercase S    |
|  54  |  T     | Uppercase T    |
|  55  |  U     | Uppercase U    |
|  56  |  V     | Uppercase V    |
|  57  |  W     | Uppercase W    |
|  58  |  X     | Uppercase X    |
|  59  |  Y     | Uppercase Y    |
|  5A  |  Z     | Uppercase Z    |
|  5B  |  [     | Left square bracket|
|  5C  |  \     | Backslash      |
|  5D  |  ]     | Right square bracket|
|  5E  |  ^     | Caret          |
|  5F  |  _     | Underscore     |
|  60  |  `     | Grave accent   |
|  61  |  a     | Lowercase a    |
|  62  |  b     | Lowercase b    |
|  63  |  c     | Lowercase c    |
|  64  |  d     | Lowercase d    |
|  65  |  e     | Lowercase e    |
|  66  |  f     | Lowercase f    |
|  67  |  g     | Lowercase g    |
|  68  |  h     | Lowercase h    |
|  69  |  i     | Lowercase i    |
|  6A  |  j     | Lowercase j    |
|  6B  |  k     | Lowercase k    |
|  6C  |  l     | Lowercase l    |
|  6D  |  m     | Lowercase m    |
|  6E  |  n     | Lowercase n    |
|  6F  |  o     | Lowercase o    |
|  70  |  p     | Lowercase p    |
|  71  |  q     | Lowercase q    |
|  72  |  r     | Lowercase r    |
|  73  |  s     | Lowercase s    |
|  74  |  t     | Lowercase t    |
|  75  |  u     | Lowercase u    |
|  76  |  v     | Lowercase v    |
|  77  |  w     | Lowercase w    |
|  78  |  x     | Lowercase x    |
|  79  |  y     | Lowercase y    |
|  7A  |  z     | Lowercase z    |
|  7B  |  {     | Left curly bracket|
|  7C  |  |     | Vertical bar   |
|  7D  |  }     | Right curly bracket|
|  7E  |  ~     | Tilde          |
|  7F  |  DEL   | Delete         |

