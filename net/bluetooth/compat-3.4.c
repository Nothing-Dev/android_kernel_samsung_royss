/*
 * Copyright 2012  Luis R. Rodriguez <mcgrof@frijolero.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Backport functionality introduced in Linux 3.4.
 */

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/wait.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
#include <linux/regmap.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)) */
// @daniel, for hid_alloc_report_buf()
#include <linux/export.h>
#include <linux/hid.h>
#include <linux/bug.h>
// @

/* daniel, backport 3.13-1, from backport-3.12.c
 * Allocator for buffer that is going to be passed to hid_output_report()
 */
u8 *hid_alloc_report_buf(struct hid_report *report, gfp_t flags)
{
	/*
	 * 7 extra bytes are necessary to achieve proper functionality
	 * of implement() working on 8 byte chunks
	 */

	int len = ((report->size - 1) >> 3) + 1 + (report->id > 0) + 7;

	return kmalloc(len, flags);
}
EXPORT_SYMBOL_GPL(hid_alloc_report_buf);
