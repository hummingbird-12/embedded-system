#ifndef _INTERRUPT_H_INCLUDED_
#define _INTERRUPT_H_INCLUDED_

#define GPIO_HOME IMX_GPIO_NR(1, 11)
#define GPIO_BACK IMX_GPIO_NR(1, 12)
#define GPIO_VOLUP IMX_GPIO_NR(2, 15)
#define GPIO_VOLDOWN IMX_GPIO_NR(5, 14)

void register_interrupts(void);
void release_interrupts(void);
void sleep_app(void);

#endif /* _INTERRUPT_H_INCLUDED_ */
