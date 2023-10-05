
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long u64;
typedef unsigned long size_t;

#define CLINT_BASE 0x2000000
#define MTIME_low (volatile u32 *)(CLINT_BASE + 0xBFF8)
#define MTIME_high (volatile u32 *)(CLINT_BASE + 0xBFFC)
#define MTIMECMP0 (volatile u32 *)(CLINT_BASE + 0x4000)
#define MTIMECMP1 (volatile u32 *)(CLINT_BASE + 0x4008)

/* UART */
#define UART0_CTRL_ADDR 0x54000000UL
#define UART_REG_TXFIFO 0x00

/* This function will read a 32-bit value from an MMIO register */
static inline u32 mmio_read_u32(unsigned long reg, unsigned int offset) {
  return (*(volatile u32 *)((reg) + (offset)));
}

/* This function will write a byte to an MMIO register */
static inline void mmio_write_u8(unsigned long reg, unsigned int offset,
                                 u8 val) {
  (*(volatile u32 *)((reg) + (offset))) = val;
}

/*This function will write a 32-bit value to an MMIO register */
static inline void mmio_write_u32(unsigned long reg, unsigned int offset,
                                  u32 val) {
  (*(volatile u32 *)((reg) + (offset))) = val;
}

/* Transmit a single byte over the UART */
static void __uart_write(u8 byte) {
  /* wait for the UART to become ready */
  while (mmio_read_u32(UART0_CTRL_ADDR, UART_REG_TXFIFO) & 0x80000000)
    ;

  /* write to the UART transmit FIFO */
  mmio_write_u8(UART0_CTRL_ADDR, UART_REG_TXFIFO, byte);
}

/* Transmit a buffer of length "len" over the UART */
static void uart_write(u8 *buf, size_t len) {
  int i;
  for (i = 0; i < len; i++) {
    __uart_write(buf[i]);
    /* If an LF was written, also write a CR */
    if (buf[i] == '\n') {
      __uart_write('\r');
    }
  }
}

static inline void enable_interrupts() {
  u64 mstatus;
  asm volatile("csrr %0, mstatus" : "=r"(mstatus));
  mstatus |= 1 << 3;
  asm volatile("csrw mstatus, %0" ::"r"(mstatus));
}

static inline void disable_interrupts() {
  u64 mstatus;
  asm volatile("csrr %0, mstatus" : "=r"(mstatus));
  mstatus &= ~(1 << 3);
  asm volatile("csrw mstatus, %0" ::"r"(mstatus));
}

static inline u64 read_hartid() {
  u64 hartid;
  asm volatile("csrr %0, mhartid" : "=r"(hartid));
  return hartid;
}

void __attribute__((interrupt)) handle_trap() {
  // disable_interrupts();

  if (read_hartid() == 0) {
    uart_write("core0\n", 6);
    *MTIMECMP0 = *MTIME_low + 10000;
  } else {
    uart_write("core1\n", 6);
    *MTIMECMP1 = *MTIME_low + 7000;
  }
}

void _init() {

  // Enable machine timer interrupts
  u64 mie;
  asm volatile("csrr %0, mie" : "=r"(mie));
  mie |= 1 << 7;
  asm volatile("csrw mie, %0" ::"r"(mie));

  enable_interrupts();

  uart_write("BOOT...\n", 8);

  *MTIMECMP0 = *MTIME_low + 10000;
  *MTIMECMP1 = *MTIME_low + 7000;

  // Main loop
  while (1) {
    // Your code here
  }
}
