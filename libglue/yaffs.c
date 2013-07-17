#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <console.h>

#include <hw/mem.h>

#include <yaffsfs.h>
#include <yaffs_guts.h>
#include <yaffs_packedtags2.h>

#include <glue.h>

/*
 * YAFFS callbacks
 */

unsigned int yaffs_trace_mask;

void yaffsfs_Lock(void)
{
	/* nothing to do */
}

void yaffsfs_Unlock(void)
{
	/* nothing to do */
}

u32 yaffsfs_CurrentTime(void)
{
	return 0;
}

void yaffsfs_SetError(int err)
{
	errno = -err;
}

int yaffsfs_GetLastError(void)
{
	return -errno;
}

void *yaffsfs_malloc(size_t size)
{
	return malloc(size);
}

void yaffsfs_free(void *ptr)
{
	free(ptr);
}

void yaffs_bug_fn(const char *file_name, int line_no)
{
	printf("YAFFS BUG in %s line %d\n", file_name, line_no);
}

/*
 * YAFFS init and flash access
 */

#define NOR_SIZE		(32*1024*1024)
#define NOR_BLOCKSIZE		(128*1024)

#define NOR_CHUNK_DATA_SIZE	512
#define NOR_CHUNK_TAGS_SIZE	16
#define NOR_CHUNK_WHOLE_SIZE	(NOR_CHUNK_DATA_SIZE+NOR_CHUNK_TAGS_SIZE)

#define FLASH_OFFSET_FILESYSTEM (FLASH_OFFSET_APP + 1024*1024*2)

static void read_flash(void *data, int len, int offset)
{
	memcpy(data, (char *)(0x80000000 | FLASH_OFFSET_FILESYSTEM) + offset, len);
}

static void write_flash(const void *data, int len, int offset)
{
	/* TODO */
}

static unsigned int chunk_address(struct yaffs_dev *dev, int c)
{
	unsigned int chunks_per_block = dev->param.chunks_per_block;
	return NOR_BLOCKSIZE*(c/chunks_per_block)
		+ NOR_CHUNK_WHOLE_SIZE*(c%chunks_per_block);
}

static int read_chunk_tags(struct yaffs_dev *dev, int nand_chunk, u8 *data, struct yaffs_ext_tags *tags)
{
	unsigned int address;
	
	//printf("%s %d (data=%p tags=%p)\n", __func__, nand_chunk, data, tags);
	address = chunk_address(dev, nand_chunk);
	if(data)
		read_flash(data, NOR_CHUNK_DATA_SIZE, address);
	if(tags) {
		struct yaffs_packed_tags2_tags_only x;
		read_flash(&x, NOR_CHUNK_TAGS_SIZE, address+NOR_CHUNK_DATA_SIZE);
		yaffs_unpack_tags2_tags_only(tags, &x);
	}
	return YAFFS_OK;
}

static int write_chunk_tags(struct yaffs_dev *dev, int nand_chunk, const u8 *data, const struct yaffs_ext_tags *tags)
{
	unsigned int address;
	
	//printf("%s %d (data=%p tags=%p)\n", __func__, nand_chunk, data, tags);
	address = chunk_address(dev, nand_chunk);
	if(data)
		write_flash(data, NOR_CHUNK_DATA_SIZE, address);
	if(tags) {
		struct yaffs_packed_tags2_tags_only x;
		yaffs_pack_tags2_tags_only(&x, tags);
		write_flash(&x, NOR_CHUNK_TAGS_SIZE, address+NOR_CHUNK_DATA_SIZE);
	}
	return YAFFS_OK;
}

static int bad_block(struct yaffs_dev *dev, int blockId)
{
	struct yaffs_ext_tags tags;
	int chunk_nr;

	chunk_nr = blockId * dev->param.chunks_per_block;

	read_chunk_tags(dev, chunk_nr, NULL, &tags);
	tags.block_bad = 1;
	write_chunk_tags(dev, chunk_nr, NULL, &tags);
	
	return YAFFS_OK;
}

static int query_block(struct yaffs_dev *dev, int blockId, enum yaffs_block_state *state, u32 *seq_number)
{
	struct yaffs_ext_tags tags;
	int chunk_nr;

	*seq_number = 0;

	chunk_nr = blockId * dev->param.chunks_per_block;

	read_chunk_tags(dev, chunk_nr, NULL, &tags);
	if(tags.block_bad)
		*state = YAFFS_BLOCK_STATE_DEAD;
	else if(!tags.chunk_used)
		*state = YAFFS_BLOCK_STATE_EMPTY;
	else if(tags.chunk_used) {
		*state = YAFFS_BLOCK_STATE_NEEDS_SCAN;
		*seq_number = tags.seq_number;
	}
	
	return YAFFS_OK;
}

static int erase(struct yaffs_dev *dev, int blockId)
{
	// TODO my_ioctl(sc->flashdev, FLASH_ERASE_BLOCK, (void *)(blockId*sc->blocksize));
	
	return YAFFS_OK;
}

static int initialise(struct yaffs_dev *dev)
{
	return YAFFS_OK;
}

static struct yaffs_dev flash_dev = {
	.read_only = 0,
	.param = {
		.name = "/",
	
		.start_block = 0,
		.end_block = NOR_SIZE/NOR_BLOCKSIZE - 1,
		.chunks_per_block = NOR_BLOCKSIZE/NOR_CHUNK_WHOLE_SIZE,
		.total_bytes_per_chunk = NOR_CHUNK_WHOLE_SIZE,
		.n_reserved_blocks = 5,
		.n_caches = 15,
		.inband_tags = 1,
		.is_yaffs2 = 1,
		.no_tags_ecc = 1,
	
		.write_chunk_tags_fn = write_chunk_tags,
		.read_chunk_tags_fn = read_chunk_tags,
		.bad_block_fn = bad_block,
		.query_block_fn = query_block,
		.erase_fn = erase,
		.initialise_flash_fn = initialise
	}
};


void fs_init(void)
{
	yaffs_add_device(&flash_dev);
	yaffs_mount("/");
}
