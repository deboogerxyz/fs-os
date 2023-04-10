
#ifndef _KERNEL_DATETIME_H
#define _KERNEL_DATETIME_H

typedef struct {
    uint8_t h; /**< @brief Hour */
    uint8_t m; /**< @brief Minute */
    uint8_t s; /**< @brief Sec */
} Time;

typedef struct {
    uint8_t d;  /**< @brief Day */
    uint8_t m;  /**< @brief Month */
    uint16_t y; /**< @brief Year */
    uint8_t c;  /**< @brief Century */
} Date;

typedef struct {
    Date date; /**< Not a pointer */
    Time time; /**< Not a pointer */
} DateTime;

#endif /* _KERNEL_DATETIME_H */

