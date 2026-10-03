#include "ext2.h"

#include "../drivers/disk/ata.h"

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

void read_inode(uint32_t start_block, ext2_super_block_t *sb, ext2_block_group_descriptor_t *bgdt, ext2_inode_t *inode, uint32_t inode_number)
{
    if (inode_number == 0) return;
    
    uint32_t block_size = 1024 << sb->s_log_block_size;
    
    uint32_t block_group = (inode_number - 1) / sb->s_inodes_per_group;
    uint32_t inode_index = (inode_number - 1) % sb->s_inodes_per_group;

    ext2_block_group_descriptor_t *bg_desc = &bgdt[block_group];
    
    // Вычисляем позицию inode в таблице
    uint32_t inode_offset = inode_index * sb->s_inode_size;
    uint32_t inode_table_block = bg_desc->inode_table;
    uint32_t block_offset = inode_offset / block_size;
    uint32_t byte_offset = inode_offset % block_size;

    uint32_t lba = (start_block + inode_table_block + block_offset) * (block_size >> 9);
    uint8_t buffer[block_size]; // <==
    ata_read_sectors(lba, block_size >> 9, buffer);

    memcpy(inode, buffer + byte_offset, sizeof(ext2_inode_t));
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
    uint32_t block_size = 1024 << sb->s_log_block_size; // Подсчёт размера блока

    read_inode(start_block, sb, bgdt, &dir_inode, inode_number); // читаем иноду папки

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