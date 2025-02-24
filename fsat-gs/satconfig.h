// rms (2025/02/19)

#ifndef _SATCONFIG_H_
#define _SATCONFIG_H_

// Telemetry
#define SAT_TELE_UART_IDX UART_IDX2
#define SAT_TELE_UART_BAUD 115200

// Start time of the threads [ms]
#define SAT_THREAD_START_SCTRL_MS 30
#define SAT_THREAD_START_MCTRL_MS 30
#define SAT_THREAD_START_TELE_MS 1000
#define SAT_THREAD_START_TCMD_MS 30
#define SAT_THREAD_START_AHRS_MS 30

// Period of the threads [ms]
#define SAT_THREAD_PERIOD_SCTRL_MS 30
#define SAT_THREAD_PERIOD_MCTRL_MS 25
#define SAT_THREAD_PERIOD_TELE_MS 100
#define SAT_THREAD_PERIOD_TCMD_MS 30
#define SAT_THREAD_PERIOD_AHRS_MS 30

// Priorities
#define SAT_THREAD_PRIO_SCTRL 100
#define SAT_THREAD_PRIO_MCTRL 100
#define SAT_THREAD_PRIO_TELE 100
#define SAT_THREAD_PRIO_TCMD 100
#define SAT_THREAD_PRIO_AHRS 100

// Motor angular rate control params
#define SAT_GAINS_MOTOR_OMEGA_KP 0.01
#define SAT_GAINS_MOTOR_OMEGA_KI 0.01

// Satellite angular rate control params
#define SAT_GAINS_SAT_OMEGA_KP 0.01
#define SAT_GAINS_SAT_OMEGA_KI 0.01

// Satellite yaw control params
#define SAT_GAINS_SAT_YAW_KP 0.01
#define SAT_GAINS_SAT_YAW_KI 0.01
#define SAT_GAINS_SAT_YAW_KD 0.01

// Control output limits
#define SAT_MOTOR_PWM_UMAX 100.0    // Max PWM
#define SAT_MOTOR_PWM_UMIN -100.0   // Min PWM
#define SAT_MOTOR_OMEGA_UMAX 100.0  // Max motor rate [deg/s]
#define SAT_MOTOR_OMEGA_UMIN -100.0 // Min motor rate [deg/s]

// Reaction wheel
#define SAT_RW_PWM1 PWM_IDX02
#define SAT_RW_PWM2 PWM_IDX03
#define SAT_RW_PWM_FREQUENCY 5000 // Hz
#define SAT_RW_PWM_INCREMENTS 1000

// Encoder
#define SAT_ENCODER_CPR 64 // Counts per revolution
#define SAT_ENCODER_LPF_FC 0.9 // Check tune_lpf.m

#endif // satconfig.h
