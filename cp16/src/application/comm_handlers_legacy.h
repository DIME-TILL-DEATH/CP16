#ifndef SRC_APPLICATION_COMM_HANDLERS_LEGACY_H_
#define SRC_APPLICATION_COMM_HANDLERS_LEGACY_H_

#include "readline.h"
#include "console.h"

inline void default_param_handler(uint8_t *param_ptr, TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	char hex[3] = { 0, 0, 0 };
	if (count > 0)
	{
		if (count == 2)
		{
			char *end;
			uint8_t val = kgp_sdk_libc::strtol(args[1], &end, 16);
			*param_ptr = val;
		}

		i2hex(*param_ptr, hex);
		msg_console("%s %s\n", args[0], hex);
	}
}

void set_legacy_handlers(TReadLine *rl);

#endif /* SRC_APPLICATION_COMM_HANDLERS_LEGACY_H_ */
