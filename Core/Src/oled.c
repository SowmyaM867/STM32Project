#include "stm32f4xx.h"
#include "oled.h"
#include <stdio.h>
#include <string.h>

#define OLED_ADDR        0x3C   /* 7-bit address (0x3D if the module is jumpered) */
#define OLED_COL_OFFSET  0      /* use 2 for an SH1106 display                    */
#define OLED_PAGES       4      /* 128x32 display = 4 pages = 4 text lines */
#define OLED_TEXT_LEN    24

/* 5x7 font, ASCII 0x20 (space) to 0x7E (~). Each character = 5 column bytes. */
static const uint8_t font5x7[95][5] =
{
    {0x00,0x00,0x00,0x00,0x00}, /* space */
    {0x00,0x00,0x5F,0x00,0x00}, /* ! */
    {0x00,0x07,0x00,0x07,0x00}, /* " */
    {0x14,0x7F,0x14,0x7F,0x14}, /* # */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* $ */
    {0x23,0x13,0x08,0x64,0x62}, /* % */
    {0x36,0x49,0x55,0x22,0x50}, /* & */
    {0x00,0x05,0x03,0x00,0x00}, /* ' */
    {0x00,0x1C,0x22,0x41,0x00}, /* ( */
    {0x00,0x41,0x22,0x1C,0x00}, /* ) */
    {0x14,0x08,0x3E,0x08,0x14}, /* * */
    {0x08,0x08,0x3E,0x08,0x08}, /* + */
    {0x00,0x50,0x30,0x00,0x00}, /* , */
    {0x08,0x08,0x08,0x08,0x08}, /* - */
    {0x00,0x60,0x60,0x00,0x00}, /* . */
    {0x20,0x10,0x08,0x04,0x02}, /* / */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 0 */
    {0x00,0x42,0x7F,0x40,0x00}, /* 1 */
    {0x42,0x61,0x51,0x49,0x46}, /* 2 */
    {0x21,0x41,0x45,0x4B,0x31}, /* 3 */
    {0x18,0x14,0x12,0x7F,0x10}, /* 4 */
    {0x27,0x45,0x45,0x45,0x39}, /* 5 */
    {0x3C,0x4A,0x49,0x49,0x30}, /* 6 */
    {0x01,0x71,0x09,0x05,0x03}, /* 7 */
    {0x36,0x49,0x49,0x49,0x36}, /* 8 */
    {0x06,0x49,0x49,0x29,0x1E}, /* 9 */
    {0x00,0x36,0x36,0x00,0x00}, /* : */
    {0x00,0x56,0x36,0x00,0x00}, /* ; */
    {0x08,0x14,0x22,0x41,0x00}, /* < */
    {0x14,0x14,0x14,0x14,0x14}, /* = */
    {0x00,0x41,0x22,0x14,0x08}, /* > */
    {0x02,0x01,0x51,0x09,0x06}, /* ? */
    {0x32,0x49,0x79,0x41,0x3E}, /* @ */
    {0x7E,0x11,0x11,0x11,0x7E}, /* A */
    {0x7F,0x49,0x49,0x49,0x36}, /* B */
    {0x3E,0x41,0x41,0x41,0x22}, /* C */
    {0x7F,0x41,0x41,0x41,0x3E}, /* D */
    {0x7F,0x49,0x49,0x49,0x41}, /* E */
    {0x7F,0x09,0x09,0x09,0x01}, /* F */
    {0x3E,0x41,0x49,0x49,0x7A}, /* G */
    {0x7F,0x08,0x08,0x08,0x7F}, /* H */
    {0x00,0x41,0x7F,0x41,0x00}, /* I */
    {0x20,0x40,0x41,0x3F,0x01}, /* J */
    {0x7F,0x08,0x14,0x22,0x41}, /* K */
    {0x7F,0x40,0x40,0x40,0x40}, /* L */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* M */
    {0x7F,0x04,0x08,0x10,0x7F}, /* N */
    {0x3E,0x41,0x41,0x41,0x3E}, /* O */
    {0x7F,0x09,0x09,0x09,0x06}, /* P */
    {0x3E,0x41,0x51,0x21,0x5E}, /* Q */
    {0x7F,0x09,0x19,0x29,0x46}, /* R */
    {0x46,0x49,0x49,0x49,0x31}, /* S */
    {0x01,0x01,0x7F,0x01,0x01}, /* T */
    {0x3F,0x40,0x40,0x40,0x3F}, /* U */
    {0x1F,0x20,0x40,0x20,0x1F}, /* V */
    {0x3F,0x40,0x38,0x40,0x3F}, /* W */
    {0x63,0x14,0x08,0x14,0x63}, /* X */
    {0x07,0x08,0x70,0x08,0x07}, /* Y */
    {0x61,0x51,0x49,0x45,0x43}, /* Z */
    {0x00,0x7F,0x41,0x41,0x00}, /* [ */
    {0x02,0x04,0x08,0x10,0x20}, /* \ */
    {0x00,0x41,0x41,0x7F,0x00}, /* ] */
    {0x04,0x02,0x01,0x02,0x04}, /* ^ */
    {0x40,0x40,0x40,0x40,0x40}, /* _ */
    {0x00,0x01,0x02,0x04,0x00}, /* ` */
    {0x20,0x54,0x54,0x54,0x78}, /* a */
    {0x7F,0x48,0x44,0x44,0x38}, /* b */
    {0x38,0x44,0x44,0x44,0x20}, /* c */
    {0x38,0x44,0x44,0x48,0x7F}, /* d */
    {0x38,0x54,0x54,0x54,0x18}, /* e */
    {0x08,0x7E,0x09,0x01,0x02}, /* f */
    {0x0C,0x52,0x52,0x52,0x3E}, /* g */
    {0x7F,0x08,0x04,0x04,0x78}, /* h */
    {0x00,0x44,0x7D,0x40,0x00}, /* i */
    {0x20,0x40,0x44,0x3D,0x00}, /* j */
    {0x7F,0x10,0x28,0x44,0x00}, /* k */
    {0x00,0x41,0x7F,0x40,0x00}, /* l */
    {0x7C,0x04,0x18,0x04,0x78}, /* m */
    {0x7C,0x08,0x04,0x04,0x78}, /* n */
    {0x38,0x44,0x44,0x44,0x38}, /* o */
    {0x7C,0x14,0x14,0x14,0x08}, /* p */
    {0x08,0x14,0x14,0x18,0x7C}, /* q */
    {0x7C,0x08,0x04,0x04,0x08}, /* r */
    {0x48,0x54,0x54,0x54,0x20}, /* s */
    {0x04,0x3F,0x44,0x40,0x20}, /* t */
    {0x3C,0x40,0x40,0x20,0x7C}, /* u */
    {0x1C,0x20,0x40,0x20,0x1C}, /* v */
    {0x3C,0x40,0x30,0x40,0x3C}, /* w */
    {0x44,0x28,0x10,0x28,0x44}, /* x */
    {0x0C,0x50,0x50,0x50,0x3C}, /* y */
    {0x44,0x64,0x54,0x4C,0x44}, /* z */
    {0x00,0x08,0x36,0x41,0x00}, /* { */
    {0x00,0x00,0x7F,0x00,0x00}, /* | */
    {0x00,0x41,0x36,0x08,0x00}, /* } */
    {0x10,0x08,0x08,0x10,0x08}  /* ~ */
};

/* ------------------------------------------------------------------ */
/*  I2C1 (PB8 = SCL, PB9 = SDA, AF4), 400 kHz, APB1 = 16 MHz            */
/* ------------------------------------------------------------------ */
void I2C1_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    (void)RCC->APB1ENR;

    /* PB8, PB9: alternate function, open-drain, high speed, pull-up */
    GPIOB->MODER   &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
    GPIOB->MODER   |=  ((2U << (8 * 2)) | (2U << (9 * 2)));
    GPIOB->OTYPER  |=  (1U << 8) | (1U << 9);
    GPIOB->OSPEEDR |=  ((3U << (8 * 2)) | (3U << (9 * 2)));
    GPIOB->PUPDR   &= ~((3U << (8 * 2)) | (3U << (9 * 2)));
    GPIOB->PUPDR   |=  ((1U << (8 * 2)) | (1U << (9 * 2)));

    /* AF4 = I2C1. Pins 8 and 9 are in AFR[1] at bit offsets 0 and 4 */
    GPIOB->AFR[1]  &= ~((0xFU << 0) | (0xFU << 4));
    GPIOB->AFR[1]  |=  ((4U   << 0) | (4U   << 4));

    /* Reset the peripheral, then configure */
    I2C1->CR1 |=  I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    I2C1->CR2   = 16;                      /* APB1 clock = 16 MHz            */
    I2C1->CCR   = I2C_CCR_FS | 14;         /* fast mode, about 380 kHz       */
    I2C1->TRISE = 5;                       /* 300 ns max rise time + 1       */

    I2C1->CR1 |= I2C_CR1_PE;               /* enable I2C1                    */
}

/* Returns 0 on success, -1 if the OLED does not answer (not connected, wrong address) */
static int I2C1_Start(uint8_t addr_w)
{
    uint32_t t;

    I2C1->CR1 |= I2C_CR1_START;
    t = 100000;
    while (!(I2C1->SR1 & I2C_SR1_SB) && --t) { }
    if (!t) return -1;

    I2C1->DR = addr_w;
    t = 100000;
    while (!(I2C1->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) && --t) { }
    if (!t || (I2C1->SR1 & I2C_SR1_AF))
    {
        I2C1->SR1 &= ~I2C_SR1_AF;          /* no ACK: clear flag and release the bus */
        I2C1->CR1 |= I2C_CR1_STOP;
        return -1;
    }

    (void)I2C1->SR1;                       /* reading SR1 then SR2 clears ADDR */
    (void)I2C1->SR2;
    return 0;
}

static int I2C1_WriteByte(uint8_t b)
{
    uint32_t t = 100000;
    while (!(I2C1->SR1 & I2C_SR1_TXE) && --t) { }
    if (!t) return -1;
    I2C1->DR = b;
    return 0;
}

static void I2C1_Stop(void)
{
    uint32_t t = 100000;
    while (!(I2C1->SR1 & I2C_SR1_BTF) && --t) { }
    I2C1->CR1 |= I2C_CR1_STOP;
    t = 100000;
    while ((I2C1->CR1 & I2C_CR1_STOP) && --t) { }
}

/* ------------------------------------------------------------------ */
/*  SSD1306                                                             */
/* ------------------------------------------------------------------ */
static void OLED_Cmd(uint8_t c)
{
    if (I2C1_Start(OLED_ADDR << 1)) return;
    I2C1_WriteByte(0x00);                  /* control byte: command          */
    I2C1_WriteByte(c);
    I2C1_Stop();
}

/* One page = 8 pixel rows = one text line. buf holds 128 column bytes. */
static void OLED_WritePage(uint8_t page, const uint8_t *buf)
{
    OLED_Cmd(0xB0 | page);                             /* set page          */
    OLED_Cmd(0x00 | (OLED_COL_OFFSET & 0x0F));         /* column low nibble */
    OLED_Cmd(0x10 | (OLED_COL_OFFSET >> 4));           /* column high nibble*/

    if (I2C1_Start(OLED_ADDR << 1)) return;
    I2C1_WriteByte(0x40);                  /* control byte: data             */
    for (uint8_t i = 0; i < 128; i++)
        I2C1_WriteByte(buf[i]);
    I2C1_Stop();
}

void OLED_Init(void)
{
    static const uint8_t init_cmds[] =
    {
        0xAE,             /* display off                         */
        0xD5, 0x80,       /* clock divide / oscillator           */
        0xA8, 0x1F,       /* multiplex ratio: 32 rows            */
        0xD3, 0x00,       /* display offset                      */
        0x40,             /* start line = 0                      */
        0x8D, 0x14,       /* charge pump on                      */
        0x20, 0x02,       /* page addressing mode                */
        0xA1,             /* segment remap (flip horizontally)   */
        0xC8,             /* COM scan direction (flip vertically)*/
        0xDA, 0x02,       /* COM pins configuration (128x32)     */
        0x81, 0x8F,       /* contrast                            */
        0xD9, 0xF1,       /* pre-charge                          */
        0xDB, 0x40,       /* VCOMH level                         */
        0xA4,             /* follow RAM content                  */
        0xA6,             /* normal (not inverted)               */
        0xAF              /* display on                          */
    };

    for (uint8_t i = 0; i < sizeof(init_cmds); i++)
        OLED_Cmd(init_cmds[i]);
}

void OLED_Clear(void)
{
    uint8_t blank[128] = {0};
    for (uint8_t page = 0; page < OLED_PAGES; page++)
        OLED_WritePage(page, blank);
}

/* Draw one line of text (up to 21 characters) on the given page */
static void OLED_DrawLine(uint8_t line, const char *str)
{
    uint8_t buf[128] = {0};
    uint8_t x = 0;

    while (*str && (x + 6) <= 128)
    {
        char c = *str++;
        if (c < 32 || c > 126) c = '?';
        memcpy(&buf[x], font5x7[c - 32], 5);   /* 5 columns of the glyph */
        x += 6;                                /* + 1 blank column       */
    }
    OLED_WritePage(line, buf);
}

/* ------------------------------------------------------------------ */
/*  Screen content                                                      */
/*  The display has 4 text lines but we have 7 items, so there are two  */
/*  screens: screen 0 = items 0 to 3, screen 1 = items 4 to 6.          */
/* ------------------------------------------------------------------ */
void OLED_Update(const SystemStatus *s, uint8_t screen)
{
    static const char *light_txt[] = { "DARK", "DIM", "BRIGHT" };
    static char shown[OLED_PAGES][OLED_TEXT_LEN];   /* text currently on each line */

    char text[OLED_TEXT_LEN];
    int16_t t   = s->temp_x10;
    int16_t abs = (t < 0) ? -t : t;
    uint8_t lt  = (s->light > 2) ? 2 : s->light;

    for (uint8_t row = 0; row < OLED_PAGES; row++)
    {
        uint8_t item = (screen ? 4 : 0) + row;      /* which of the 7 items */
        text[0] = '\0';                             /* item 7 = blank line  */

        switch (item)
        {
        case 0:
            snprintf(text, sizeof(text), "Temperature : %s%d.%d C",
                     (t < 0) ? "-" : "", abs / 10, abs % 10);
            break;
        case 1:
            snprintf(text, sizeof(text), "Light       : %s", light_txt[lt]);
            break;
        case 2:
            snprintf(text, sizeof(text), "Home Light  : %s", s->home_light ? "ON" : "OFF");
            break;
        case 3:
            snprintf(text, sizeof(text), "Buzzer      : %s", s->buzzer ? "ON" : "OFF");
            break;
        case 4:
            snprintf(text, sizeof(text), "Motor       : %s", s->motor ? "ON" : "OFF");
            break;
        case 5:
            snprintf(text, sizeof(text), "Motor Speed : %u%%", (unsigned)s->motor_speed);
            break;
        case 6:
            snprintf(text, sizeof(text), "System      : %s", s->cooling ? "COOLING" : "NORMAL");
            break;
        default:
            break;
        }

        if (strcmp(text, shown[row]) != 0)          /* redraw only if this line changed */
        {
            OLED_DrawLine(row, text);
            strcpy(shown[row], text);
        }
    }
}
