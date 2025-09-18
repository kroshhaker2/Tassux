#include "ext2.h"

#include "ata.h"

void read_superblock (uint32_t start_block, ext2_super_block_t *sb)
{
    uint8_t buffer[1024];
    ata_read_sectors(start_block + 2, 2, buffer);
    memcpy(sb, buffer, sizeof(ext2_super_block_t));
}

void read_bg_desc (uint32_t start_block, ext2_block_group_descriptor_t *bgdt)
{
    uint8_t buffer[512];
    ata_read_sector(start_block + 4, buffer);
    memcpy(bgdt, buffer, sizeof(ext2_block_group_descriptor_t));
}

void read_inode (uint32_t start_block, ext2_super_block_t *sb, ext2_block_group_descriptor_t *bgdt, ext2_inode_t *inode, uint8_t n)
{
    if (n == 0 || n > sb->s_inodes_per_group) return;
    uint32_t block_size = 1024 << sb->s_log_block_size;
    uint32_t lba = start_block + (bgdt->inode_table * (block_size >> 9)) + ((sb->s_inode_size * (n - 1)) / 512);
    uint8_t buffer[1024];
    ata_read_sectors(lba, 2, buffer);
    uint16_t offset = sb->s_inode_size * (n - 1) & 0x1FF;
    uint8_t inode_table[128];
    for (uint8_t i = 0; i < 128; i++) {
        inode_table[i] = buffer[offset + i];
    }
    memcpy(inode, inode_table, sizeof(inode_table));
}

// type = 0 (block bitmap), type = 1 (inode bitmap)
void read_bitmap(uint32_t start_block, uint32_t block_size, ext2_block_group_descriptor_t *bgdt, uint8_t type, uint8_t *buffer)
{
    uint32_t bitmap_block = (type == 0) ? bgdt->block_bitmap : bgdt->inode_bitmap;
    uint32_t lba = start_block + bitmap_block * (block_size >> 9);
    ata_read_sectors(lba, block_size >> 9, buffer);
}

int test_bit(uint8_t *bitmap, uint32_t index)
{
    return (bitmap[index / 8] >> (index % 8)) & 1;
}

// value: 1 = занять, 0 = освободить
void set_bit(uint32_t start_block, uint32_t block_size, ext2_block_group_descriptor_t *bgdt, uint8_t type, uint8_t *bitmap, uint32_t index, uint8_t value)
{
    uint32_t bitmap_block = (type == 0) ? bgdt->block_bitmap : bgdt->inode_bitmap;
    uint32_t lba = start_block + (bitmap_block * (block_size >> 9));
    if (value)
        bitmap[index >> 3] |=  (1 << (index & 7));
    else
        bitmap[index >> 3] &= ~(1 << (index & 7));
    ata_write_sectors(lba, block_size >> 9, bitmap);
}

void read_dir(uint32_t start_block, ext2_super_block_t *sb, ext2_block_group_descriptor_t *bgdt, uint32_t inode_number, vfs_node_t *entries,size_t max_entries, size_t *count_out)
{
    *count_out = 0;
    ext2_inode_t dir_inode;
    uint32_t block_size = 1024 << sb->s_log_block_size;

    read_inode(start_block, sb, bgdt, &dir_inode, inode_number);

    for (int i = 0; i < 12 && *count_out < max_entries; i++) {
        if (dir_inode.block[i] == 0) continue;

        uint32_t lba = start_block + dir_inode.block[i] * (block_size >> 9);
        uint8_t buffer[block_size];
        ata_read_sectors(lba, block_size >> 9, buffer);

        uint32_t offset = 0;
        while (offset < block_size) {
            ext2_dir_entry_t *entry = (ext2_dir_entry_t *)(buffer + offset);

            if (entry->inode != 0 && *count_out < max_entries) {
                vfs_node_t *node = &entries[*count_out];
                strncpy(node->name, entry->name, entry->name_len);
                node->name[entry->name_len] = '\0';
                node->inode = entry->inode;
                node->type = entry->file_type;
                node->size = 0;
                (*count_out)++;
            }

            offset += entry->rec_len;
            if (entry->rec_len < 8) break;
        }
    }
}