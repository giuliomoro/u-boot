/*
 * Function to read values from the device tree node attached to a udevice.
 *
 * Copyright (c) 2017 Google, Inc
 * Written by Simon Glass <sjg@chromium.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _DM_READ_H
#define _DM_READ_H

#include <dm/fdtaddr.h>
#include <dm/ofnode.h>
#include <dm/uclass.h>

#if CONFIG_IS_ENABLED(OF_LIVE)
static inline const struct device_node *dev_np(struct udevice *dev)
{
	return ofnode_to_np(dev->node);
}
#else
static inline const struct device_node *dev_np(struct udevice *dev)
{
	return NULL;
}
#endif

/**
 * dev_ofnode() - get the DT node reference associated with a udevice
 *
 * @dev:	device to check
 * @return reference of the the device's DT node
 */
static inline ofnode dev_ofnode(struct udevice *dev)
{
	return dev->node;
}

static inline bool dev_of_valid(struct udevice *dev)
{
	return ofnode_valid(dev_ofnode(dev));
}

#ifdef CONFIG_DM_DEV_READ_INLINE

static inline int dev_read_u32_default(struct udevice *dev,
				       const char *propname, int def)
{
	return ofnode_read_u32_default(dev_ofnode(dev), propname, def);
}

/**
 * dev_read_string() - Read a string from a device's DT property
 *
 * @dev:	device to read DT property from
 * @propname:	name of the property to read
 * @return string from property value, or NULL if there is no such property
 */
static inline const char *dev_read_string(struct udevice *dev,
					  const char *propname)
{
	return ofnode_read_string(dev_ofnode(dev), propname);
}

/**
 * dev_read_bool() - read a boolean value from a device's DT property
 *
 * @dev:	device to read DT property from
 * @propname:	name of property to read
 * @return true if property is present (meaning true), false if not present
 */
static inline bool dev_read_bool(struct udevice *dev, const char *propname)
{
	return ofnode_read_bool(dev_ofnode(dev), propname);
}

/**
 * dev_read_subnode() - find a named subnode of a device
 *
 * @dev:	device whose DT node contains the subnode
 * @subnode_name: name of subnode to find
 * @return reference to subnode (which can be invalid if there is no such
 * subnode)
 */
static inline ofnode dev_read_subnode(struct udevice *dev,
				      const char *subbnode_name)
{
	return ofnode_find_subnode(dev_ofnode(dev), subbnode_name);
}

/**
 * dev_read_size() - read the size of a property
 *
 * @dev: device to check
 * @propname: property to check
 * @return size of property if present, or -EINVAL if not
 */
static inline int dev_read_size(struct udevice *dev, const char *propname)
{
	return ofnode_read_size(dev_ofnode(dev), propname);
}

/**
 * dev_read_addr_index() - Get the indexed reg property of a device
 *
 * @dev: Device to read from
 * @index: the 'reg' property can hold a list of <addr, size> pairs
 *	   and @index is used to select which one is required
 *
 * @return address or FDT_ADDR_T_NONE if not found
 */
static inline fdt_addr_t dev_read_addr_index(struct udevice *dev, int index)
{
	return devfdt_get_addr_index(dev, index);
}

/**
 * dev_read_addr() - Get the reg property of a device
 *
 * @dev: Device to read from
 *
 * @return address or FDT_ADDR_T_NONE if not found
 */
static inline fdt_addr_t dev_read_addr(struct udevice *dev)
{
	return devfdt_get_addr(dev);
}

/**
 * dev_read_addr_size() - get address and size from a device property
 *
 * This does no address translation. It simply reads an property that contains
 * an address and a size value, one after the other.
 *
 * @dev: Device to read from
 * @propname: property to read
 * @sizep: place to put size value (on success)
 * @return address value, or FDT_ADDR_T_NONE on error
 */
static inline fdt_addr_t dev_read_addr_size(struct udevice *dev,
					    const char *propname,
					    fdt_size_t *sizep)
{
	return ofnode_get_addr_size(dev_ofnode(dev), propname, sizep);
}

/**
 * dev_read_name() - get the name of a device's node
 *
 * @node: valid node to look up
 * @return name of node
 */
static inline const char *dev_read_name(struct udevice *dev)
{
	return ofnode_get_name(dev_ofnode(dev));
}

/**
 * dev_read_stringlist_search() - find string in a string list and return index
 *
 * Note that it is possible for this function to succeed on property values
 * that are not NUL-terminated. That's because the function will stop after
 * finding the first occurrence of @string. This can for example happen with
 * small-valued cell properties, such as #address-cells, when searching for
 * the empty string.
 *
 * @dev: device to check
 * @propname: name of the property containing the string list
 * @string: string to look up in the string list
 *
 * @return:
 *   the index of the string in the list of strings
 *   -ENODATA if the property is not found
 *   -EINVAL on some other error
 */
static inline int dev_read_stringlist_search(struct udevice *dev,
					     const char *propname,
					     const char *string)
{
	return ofnode_stringlist_search(dev_ofnode(dev), propname, string);
}

/**
 * dev_read_phandle_with_args() - Find a node pointed by phandle in a list
 *
 * This function is useful to parse lists of phandles and their arguments.
 * Returns 0 on success and fills out_args, on error returns appropriate
 * errno value.
 *
 * Caller is responsible to call of_node_put() on the returned out_args->np
 * pointer.
 *
 * Example:
 *
 * phandle1: node1 {
 *	#list-cells = <2>;
 * }
 *
 * phandle2: node2 {
 *	#list-cells = <1>;
 * }
 *
 * node3 {
 *	list = <&phandle1 1 2 &phandle2 3>;
 * }
 *
 * To get a device_node of the `node2' node you may call this:
 * dev_read_phandle_with_args(dev, "list", "#list-cells", 0, 1, &args);
 *
 * @dev:	device whose node containing a list
 * @list_name:	property name that contains a list
 * @cells_name:	property name that specifies phandles' arguments count
 * @cells_count: Cell count to use if @cells_name is NULL
 * @index:	index of a phandle to parse out
 * @out_args:	optional pointer to output arguments structure (will be filled)
 * @return 0 on success (with @out_args filled out if not NULL), -ENOENT if
 *	@list_name does not exist, -EINVAL if a phandle was not found,
 *	@cells_name could not be found, the arguments were truncated or there
 *	were too many arguments.
 */
static inline int dev_read_phandle_with_args(struct udevice *dev,
		const char *list_name, const char *cells_name, int cell_count,
		int index, struct ofnode_phandle_args *out_args)
{
	return ofnode_parse_phandle_with_args(dev_ofnode(dev), list_name,
					      cells_name, cell_count, index,
					      out_args);
}

/**
 * dev_read_addr_cells() - Get the number of address cells for a device's node
 *
 * This walks back up the tree to find the closest #address-cells property
 * which controls the given node.
 *
 * @dev: devioe to check
 * @return number of address cells this node uses
 */
static inline int dev_read_addr_cells(struct udevice *dev)
{
	return fdt_address_cells(gd->fdt_blob, dev_of_offset(dev));
}

/**
 * dev_read_size_cells() - Get the number of size cells for a device's node
 *
 * This walks back up the tree to find the closest #size-cells property
 * which controls the given node.
 *
 * @dev: devioe to check
 * @return number of size cells this node uses
 */
static inline int dev_read_size_cells(struct udevice *dev)
{
	return fdt_size_cells(gd->fdt_blob, dev_of_offset(dev));
}

/**
 * dev_read_phandle() - Get the phandle from a device
 *
 * @dev: device to check
 * @return phandle (1 or greater), or 0 if no phandle or other error
 */
static inline int dev_read_phandle(struct udevice *dev)
{
	return fdt_get_phandle(gd->fdt_blob, dev_of_offset(dev));
}

/**
 * dev_read_prop()- - read a property from a device's node
 *
 * @dev: device to check
 * @propname: property to read
 * @lenp: place to put length on success
 * @return pointer to property, or NULL if not found
 */
static inline const u32 *dev_read_prop(struct udevice *dev,
				       const char *propname, int *lenp)
{
	return ofnode_read_prop(dev_ofnode(dev), propname, lenp);
}

/**
 * dev_read_alias_seq() - Get the alias sequence number of a node
 *
 * This works out whether a node is pointed to by an alias, and if so, the
 * sequence number of that alias. Aliases are of the form <base><num> where
 * <num> is the sequence number. For example spi2 would be sequence number 2.
 *
 * @dev: device to look up
 * @devnump: set to the sequence number if one is found
 * @return 0 if a sequence was found, -ve if not
 */
static inline int dev_read_alias_seq(struct udevice *dev, int *devnump)
{
	return fdtdec_get_alias_seq(gd->fdt_blob, dev->uclass->uc_drv->name,
				    dev_of_offset(dev), devnump);
}

/**
 * dev_read_u32_array() - Find and read an array of 32 bit integers
 *
 * Search for a property in a device node and read 32-bit value(s) from
 * it.
 *
 * The out_values is modified only if a valid u32 value can be decoded.
 *
 * @dev: device to look up
 * @propname:	name of the property to read
 * @out_values:	pointer to return value, modified only if return value is 0
 * @sz:		number of array elements to read
 * @return 0 on success, -EINVAL if the property does not exist, -ENODATA if
 * property does not have a value, and -EOVERFLOW if the property data isn't
 * large enough.
 */
static inline int dev_read_u32_array(struct udevice *dev, const char *propname,
				     u32 *out_values, size_t sz)
{
	return ofnode_read_u32_array(dev_ofnode(dev), propname, out_values, sz);
}

/**
 * dev_read_first_subnode() - find the first subnode of a device's node
 *
 * @dev: device to look up
 * @return reference to the first subnode (which can be invalid if the device's
 * node has no subnodes)
 */
static inline ofnode dev_read_first_subnode(struct udevice *dev)
{
	return ofnode_first_subnode(dev_ofnode(dev));
}

/**
 * ofnode_next_subnode() - find the next sibling of a subnode
 *
 * @node:	valid reference to previous node (sibling)
 * @return reference to the next subnode (which can be invalid if the node
 * has no more siblings)
 */
static inline ofnode dev_read_next_subnode(ofnode node)
{
	return ofnode_next_subnode(node);
}

/**
 * dev_read_u8_array_ptr() - find an 8-bit array
 *
 * Look up a device's node property and return a pointer to its contents as a
 * byte array of given length. The property must have at least enough data
 * for the array (count bytes). It may have more, but this will be ignored.
 * The data is not copied.
 *
 * @dev: device to look up
 * @propname: name of property to find
 * @sz: number of array elements
 * @return pointer to byte array if found, or NULL if the property is not
 *		found or there is not enough data
 */
static inline const uint8_t *dev_read_u8_array_ptr(struct udevice *dev,
					const char *propname, size_t sz)
{
	return ofnode_read_u8_array_ptr(dev_ofnode(dev), propname, sz);
}

#endif /* CONFIG_DM_DEV_READ_INLINE */

/**
 * dev_for_each_subnode() - Helper function to iterate through subnodes
 *
 * This creates a for() loop which works through the subnodes in a device's
 * device-tree node.
 *
 * @subnode: ofnode holding the current subnode
 * @dev: device to use for interation (struct udevice *)
 */
#define dev_for_each_subnode(subnode, dev) \
	for (subnode = dev_read_first_subnode(dev); \
	     ofnode_valid(subnode); \
	     subnode = ofnode_next_subnode(subnode))

#endif