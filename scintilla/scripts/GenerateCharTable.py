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

    def MakeUTF8ClassifyMask(ch):
        mask = BytesFromLead(ch)
        if UTF8IsLeadByte(ch):
            mask |= 1 << 3
        if UTF8IsTrailByte(ch):
            mask |= 1 << 4

        # UTF8-3 first
        if ch == 0xE0:
            mask |= 1 << 5
        if (ch >= 0xE1 and ch <= 0xEC) or (ch >= 0xEE and ch <= 0xEF):
            mask |= 1 << 6
        if ch == 0xED:
            mask |= 1 << 7
        # UTF8-3 second
        if ch >= 0xA0 and ch <= 0xBF:
            mask |= 1 << 11
        if ch >= 0x80 and ch <= 0x9F:
            mask |= 1 << 12
        # UTF8-4 first
        if ch == 0xF0:
            mask |= 1 << 8
        if ch >= 0xF1 and ch <= 0xF3:
            mask |= 1 << 9
        if ch == 0xF4:
            mask |= 1 << 10
        # UTF8-4 second
        if ch >= 0x90 and ch <= 0xBF:
            mask |= 1 << 13
        if ch >= 0x80 and ch <= 0x8F:
            mask |= 1 << 14

        return mask

    UTF8ClassifyTable = []
    for i in range(0, 255, 16):
        line = ', '.join('0x%04X' % MakeUTF8ClassifyMask(ch) for ch in range(i, i + 16)) + ','
        line += ' // %02X - %02X' % (i, i + 15)
        UTF8ClassifyTable.append(line)

    print('UTF8ClassifyTable:', len(UTF8ClassifyTable))
    print('\n'.join(UTF8ClassifyTable))

if __name__ == '__main__':
    GenerateUTF8Table()
