# ============================================================================
#  Makefile - He thong phat hien dong dat AT89S52 + ADXL345 + SSD1306  
#  Compiler: SDCC (Small Device C Compiler)
#
#  Su dung:
#    make        - Bien dich va tao file .hex (1 lenh duy nhat)
#    make clean  - Xoa cac file trung gian
# ============================================================================

CC       = sdcc
PACKIHX  = packihx

# -mmcs51:          Target 8051 family
# --model-small:    Small memory model (data in internal RAM)
# --opt-code-size:  Toi uu kich thuoc code (quan trong cho 8KB flash)
CFLAGS   = -mmcs51 --model-small --opt-code-size -Iinc
LFLAGS   = -mmcs51 --model-small --opt-code-size

TARGET   = main

# Danh sach source files
# NOTE: ssd1306.c bi vo hieu hoa — AT89S52 chi co 256 byte RAM, khong du
#       de chay OLED driver dong thoi voi ADXL345 + filter + detect.
SRCS = main.c i2c.c adxl345.c filter.c detect.c buzzer.c
# SRCS += ssd1306.c    # Bo comment dong nay khi can kich hoat lai OLED

# Suy ra danh sach .rel tu .c
RELS = $(SRCS:.c=.rel)

# ============================================================================
#  Targets
# ============================================================================

# Target mac dinh: tao file .hex tu 1 lenh "make"
all: $(TARGET).hex
	@echo === Build thanh cong: $(TARGET).hex ===

# Bien dich tung file .c thanh .rel
# SDCC tu dong tao .rel trong thu muc hien tai
%.rel: src/%.c
	$(CC) $(CFLAGS) -c $<

# Link cac file .rel thanh .ihx (Intel HEX intermediate)
$(TARGET).ihx: $(RELS)
	$(CC) $(LFLAGS) $(RELS) -o $@

# Chuyen .ihx thanh .hex (Intel HEX cuoi cung, dung de nap vao MCU)
$(TARGET).hex: $(TARGET).ihx
	$(PACKIHX) $< > $@

# Xoa cac file trung gian va output (Windows compatible)
clean:
	-del /Q *.asm *.ihx *.lk *.lst *.map *.mem *.rel *.rst *.sym *.hex 2>nul

.PHONY: all clean