/********************************************************************
 ** Copyright(c) 2016, luzeya
 ** All rights reserved
 **
 ** 文件名：Netutils.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2016-12-04
 **
 ** 描  述：网络基础库：校验、大小端、ip转换、mac转换
 ** 注  意：1.
 ********************************************************************/
#include <cstring>
#include <cctype>
#include <cstdio>

#include "Netutils.h"

static const char *octet2dec[] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12",
    "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23",
    "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34",
    "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45",
    "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56",
    "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67",
    "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78",
    "79", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "100",
    "101", "102", "103", "104", "105", "106", "107", "108", "109",
    "110", "111", "112", "113", "114", "115", "116", "117", "118",
    "119", "120", "121", "122", "123", "124", "125", "126", "127",
    "128", "129", "130", "131", "132", "133", "134", "135", "136",
    "137", "138", "139", "140", "141", "142", "143", "144", "145",
    "146", "147", "148", "149", "150", "151", "152", "153", "154",
    "155", "156", "157", "158", "159", "160", "161", "162", "163",
    "164", "165", "166", "167", "168", "169", "170", "171", "172",
    "173", "174", "175", "176", "177", "178", "179", "180", "181",
    "182", "183", "184", "185", "186", "187", "188", "189", "190",
    "191", "192", "193", "194", "195", "196", "197", "198", "199",
    "200", "201", "202", "203", "204", "205", "206", "207", "208",
    "209", "210", "211", "212", "213", "214", "215", "216", "217",
    "218", "219", "220", "221", "222", "223", "224", "225", "226",
    "227", "228", "229", "230", "231", "232", "233", "234", "235",
    "236", "237", "238", "239", "240", "241", "242", "243", "244",
    "245", "246", "247", "248", "249", "250", "251", "252", "253",
    "254", "255"
};

static char dec2hex[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

void Net::Evert(void *data, int len)
{
    int before = 0, after = len - 1;
    uint8_t *p = (uint8_t*)data;

    for (; before < after; before++, after--) {
        uint8_t temp = p[after];
        p[after]  = p[before];
        p[before] = temp;
    }
}

uint16_t Net::Checksum(void *data, int len, uint16_t chksum)
{
    return ~ChecksumFold(data, len, chksum);
}

// 伪首部的校验和
inline
uint16_t Net::ChecksumPseudo(uint32_t src,  uint32_t dst,
                             uint8_t proto, uint16_t total,
                             uint16_t chksum)
{
    unsigned int sum = chksum;
    sum += (src & 0xffff) + ((src & 0xffff0000) >> 16);
    sum += (dst & 0xffff) + ((dst & 0xffff0000) >> 16);
    sum += total;
    sum += (uint16_t)proto;
    sum = (sum & 0xffff) + (sum >> 16);
    return ~(uint16_t)sum;
}

uint16_t Net::ChecksumPseudo(void *data,    int len,
                             uint32_t src,  uint32_t dst,
                             uint8_t proto, uint16_t total,
                             uint16_t chksum)
{
    chksum = ChecksumFold(data, len, chksum);
    return ChecksumPseudo(src, dst, proto, total, chksum);
}

inline
uint16_t Net::ChecksumFold(void *data, int len, uint16_t chksum)
{
    unsigned int sum = chksum;
    uint16_t *addr16 = (uint16_t*)data;

    /**
     * 每16位相加（相加的进位需要加回）
     * 最后求反
     */
    while (len > 1) {
        sum += *addr16++;
        /* 这样做防止需要加的数字太多,使sum超过32位 */
        if (sum >= 0x10000) {
            sum = (sum & 0xffff) + (sum >> 16);
        }
        len -= 2;
    }
    if (len > 0) {
        sum += *(uint8_t *)addr16;
    }
    return (uint16_t)sum;
}

uint32_t Net::IP4Addr(const char *src)
{
    static const char digits[] = "0123456789";
    int saw_digit, octets, ch;
    uint8_t *tp;
    uint32_t tmp = 0;

    saw_digit = 0;
    octets = 0;
    *(tp = (uint8_t*)&tmp) = 0;
    while ((ch = *src++) != '\0') {
        const char *pch;

        if ((pch = strchr(digits, ch)) != NULL) {
            uint32_t neww = *tp * 10 + (pch - digits);

            if (saw_digit && *tp == 0)
                return (0);
            if (neww > 255)
                return (0);
            *tp = neww;
            if (!saw_digit) {
                if (++octets > 4)
                    return (0);
                saw_digit = 1;
            }
        } else if (ch == '.' && saw_digit) {
            if (octets == 4)
                return (0);
            *++tp = 0;
            saw_digit = 0;
        } else
            return (0);
    }
    if (octets < 4)
        return (0);
    return (tmp);
}

const char* Net::IP4Addr(uint32_t ip, const char *buf)
{
    const char *d;
    char *p = (char*)buf;
    uint8_t *data = (uint8_t *)&ip;

    for (int i = 0; i < 4; i++) {
        for (d = octet2dec[data[i]]; (*p = *d) != '\0'; d++, p++)
            ;
        *p++ = '.';
    }
    p[-1] = '\0';

    return (buf);
}

const char* Net::IP4Addr(uint32_t ip)
{
    static char buf[16];
    IP4Addr(ip, buf);
    return buf;
}

static inline
int Hex(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return ch - '0';
    ch = tolower((int)ch);
    if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    return -1;
}

uint8_t* Net::MACAddr(const char *s, uint8_t *mac)
{
    /* Don't dirty result unless string is valid MAC. */
    for (int i = 0; i < 6; i++) {
        if (!isxdigit((int)s[i * 3]) || !isxdigit((int)s[i * 3 + 1]))
            return mac;
        if (i != 5 && s[i * 3 + 2] != ':')
            return mac;
    }
    for (int i = 0; i < 6; i++) {
        mac[i] = (Hex(s[i * 3]) << 4) | Hex(s[i * 3 + 1]);
    }
    return mac;
}

const char* Net::MACAddr(uint8_t *mac, const char *buf)
{
    char *p = (char*)buf;

    for (int i = 0; i < 6; i++) {
        uint8_t byte = mac[i];

        // 第一个符号
        *p++ = dec2hex[(byte & 0xf0) >> 4];

        // 第二个符号
        *p++ = dec2hex[(byte & 0xf)];

        // 冒号
        if (i != 5)
            *p++ = ':';
    }
    *p = '\0';
    return buf;
}

const char* Net::MACAddr(uint8_t *mac)
{
    static char buf[18];
    MACAddr(mac, buf);
    return buf;
}

void Net::Hexdump(void *data, int len)
{
    // 每行显示的字符个数
    int showlen = 0;
    char str[16] = {' '};
    int line = ((uint32_t)len >> 4);

    if ((len & 15) != 0)
        line++;

    for (int i = 0; i < len; i++) {
        uint8_t byte = ((char*)data)[i];

        // 第二个符号
        printf("%c", dec2hex[(byte & 0xf0) >> 4]);
        // 第一个符号
        printf("%c", dec2hex[(byte & 0xf)]);
        // 空格
        printf(" ");
        // 填充
        if ((i == (len - 1)) && (showlen < 15)) {
            printf(" ");
            for (int i = 0; i < (15 - showlen); i++)
                printf("   ");
        }
        // 输出值
        str[showlen] = (isprint(byte)) ? byte : '.';
        showlen++;
        // 换行
        if (((i + 1) & 15) == 0) {
            printf(" %.*s", 8, str);
            printf(" %.*s", 8, str + 8);
            printf("\n");
            showlen = 0;
            if (i == (len - 1)) return;
        }
    }

    // 对于不足16的进行换行
    for (int i = showlen; i <= 15; i++)
        str[i] = ' ';
    printf("%.*s", 8, str);
    printf("%.*s", 8, str + 8);
    printf("\n");
}

char* Net::Hexdump(void *data, int len, char *buffer)
{
    // 每行显示的字符个数
    int showlen = 0;
    char str[16] = {' '};
    int line = ((uint32_t)len >> 4);
    int total = 0;

    if ((len & 15) != 0)
        line++;

    for (int i = 0; i < len; i++) {
        uint8_t byte = ((char*)data)[i];

        // 第二个符号
        total += sprintf(buffer + total, "%c", dec2hex[(byte & 0xf0) >> 4]);
        // 第一个符号
        total += sprintf(buffer + total, "%c", dec2hex[(byte & 0xf)]);
        // 空格
        total += sprintf(buffer + total, " ");
        // 填充
        if ((i == (len - 1)) && (showlen < 15)) {
            total += sprintf(buffer + total, " ");
            for (int i = 0; i < (15 - showlen); i++)
                total += sprintf(buffer + total, "   ");
        }
        // 输出值
        str[showlen] = (isprint(byte)) ? byte : '.';
        showlen++;
        // 换行
        if (((i + 1) & 15) == 0) {
            total += sprintf(buffer + total, " %.*s", 8, str);
            total += sprintf(buffer + total, " %.*s", 8, str + 8);
            total += sprintf(buffer + total, "\n");
            showlen = 0;
            if (i == (len - 1)) return buffer;
        }
    }

    // 对于不足16的进行换行
    for (int i = showlen; i <= 15; i++)
        str[i] = ' ';
    total += sprintf(buffer + total, "%.*s", 8, str);
    total += sprintf(buffer + total, "%.*s", 8, str + 8);
    total += sprintf(buffer + total, "\n");
    return buffer;
}