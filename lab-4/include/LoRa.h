#ifndef LORA_H
#define LORA_H

#include <stdint.h>
#define MAX_LORA_STRING 255

bool send_command(const char *cmd, const char *expected_response,
                  char *response_buffer, uint32_t timeout_ms);

#endif // !LORA_H
