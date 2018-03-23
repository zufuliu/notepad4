#!/usr/bin/env python3

def GenerateUTF8Table():
    def BytesFromLead(leadByte):
        # Single byte or invalid
        if leadByte < 0xC2:
            return 1
        if leadByte < 0xE0:
            return 2
        if leadByte < 0xF0:
            return 3
        if leadByte < 0xF5:
            return 4
        # Characters longer than 4 bytes not possible in current UTF-8
        return 1

    def UTF8IsLeadByte(ch):
        if ch <= 0x7F:
            return True
        if ch >= 0xC2 and ch <= 0xF4:
            return True
        return False

    def UTF8IsTrailByte(ch):
        return ch >= 0x80 and ch <= 0xBF

    def UTF8IntervalNumber(ch):
        # UTF8-1
        if ch <= 0x7F:
            return 0
        # UTF8-tail
        if ch <= 0x8F:
            return 1
        if ch <= 0x9F:
            return 2
        if ch <= 0xBF:
            return 3
        # UTF8-invalid
        if ch == 0xC0 or ch == 0xC1:
            return 11
        # UTF8-2
        if ch >= 0xC2 and ch <= 0xDF:
            return 4
        # UTF8-3
        if ch == 0xE0:
            return 5
        if ch >= 0xE1 and ch <= 0xEC:
            return 6
        if ch == 0xED:
            return 7
        if ch == 0xEE or ch == 0xEF:
            return 6
        # UTF8-4
        if ch == 0xF0:
            return 8
        if ch >= 0xF1 and ch <= 0xF3:
            return 9
        if ch == 0xF4:
            return 10
        # UTF8-invalid
        return 11

    def MakeUTF8ClassifyMask(ch):
        mask = BytesFromLead(ch)
        if UTF8IsTrailByte(ch):
            mask |= 1 << 3

        number = UTF8IntervalNumber(ch)
        mask |= number << 4
        return mask

    UTF8ClassifyTable = []
    for i in range(0, 255, 16):
        line = ', '.join('0x%02X' % MakeUTF8ClassifyMask(ch) for ch in range(i, i + 16)) + ','
        line += ' // %02X - %02X' % (i, i + 15)
        UTF8ClassifyTable.append(line)

    print('UTF8ClassifyTable:', len(UTF8ClassifyTable))
    print('\n'.join(UTF8ClassifyTable))

if __name__ == '__main__':
    GenerateUTF8Table()
