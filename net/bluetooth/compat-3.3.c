/*
 * Copyright 2012  Luis R. Rodriguez <mcgrof@frijolero.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Backport functionality introduced in Linux 3.3.
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <net/dst.h>
#include <net/xfrm.h>

static DEFINE_SPINLOCK(wq_name_lock);
static LIST_HEAD(wq_name_list);

struct wq_name {
	struct list_head list;
	struct workqueue_struct *wq;
	char name[24];
};

struct workqueue_struct *
backport_alloc_workqueue(const char *fmt, unsigned int flags,
			 int max_active, struct lock_class_key *key,
			 const char *lock_name, ...)
{
	struct workqueue_struct *wq;
	struct wq_name *n = kzalloc(sizeof(*n), GFP_KERNEL);
	va_list args;

	if (!n)
		return NULL;

	va_start(args, lock_name);
	vsnprintf(n->name, sizeof(n->name), fmt, args);
	va_end(args);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	wq = __create_workqueue_key(n->name, max_active == 1, 0,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
				    0,
#endif
				    key, lock_name);
#else
	wq = __alloc_workqueue_key(n->name, flags, max_active, key, lock_name);
#endif
	if (!wq) {
		kfree(n);
		return NULL;
	}

	n->wq = wq;
	spin_lock(&wq_name_lock);
	list_add(&n->list, &wq_name_list);
	spin_unlock(&wq_name_lock);

	return wq;
}
EXPORT_SYMBOL_GPL(backport_alloc_workqueue);
