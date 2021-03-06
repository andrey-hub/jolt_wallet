/* Jolt Wallet - Open Source Cryptocurrency Hardware Wallet
 Copyright (C) 2018  Brian Pugh, James Coxon, Michael Smaili
 https://www.joltwallet.com/
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include "esp_log.h"
#include "menu8g2.h"
#include "../vault.h"
#include "submenus.h"
#include "../globals.h"
#include "../wifi.h"
#include "../loading.h"
#include "../helpers.h"
#include "../uart.h"
#include "sodium.h"
#include <string.h>

#include "nano_lib.h"

static void shuffle_arr(uint8_t *arr, int arr_len) {
    /* Fisher Yates random shuffling */
    uint8_t tmp;
    for(int i=arr_len-1; i>0; i--) {
        uint32_t idx = randombytes_random() % (i+1);
        tmp = arr[idx];
        arr[idx] = arr[i];
        arr[i] = tmp;
    }
    sodium_memzero(&tmp, sizeof(uint8_t));
}

void menu_mnemonic_restore(menu8g2_t *prev){
    const char title[] = "Restore";

    menu8g2_t menu;
    menu8g2_copy(&menu, prev);

    // Generate Random Order for user to input mnemonic
    CONFIDENTIAL uint8_t index[24];
    for(uint8_t i=0; i< sizeof(index); i++){
        index[i] = i;
    }
    shuffle_arr(index, sizeof(index));

    // Get Out of Order Mnemonic Words from UART
    CONFIDENTIAL char user_words[24][11];
    char buf[80];

    loading_enable();
    esp_log_level_set("*", ESP_LOG_ERROR);
    for(uint8_t i=0; i < sizeof(index); i++){
        uint8_t j = index[i];
        // Humans like 1-indexing
        snprintf(buf, sizeof(buf), "Word %d", j + 1);
        loading_text_title(buf, title);

        flush_uart();
        printf("\nEnter Mnemonic Word: ");
        get_serial_input(user_words[j], sizeof(user_words[j]));

        // verify its a word in the word list
        while(-1 == nl_search_wordlist(user_words[j], strlen(user_words[j]))){
            flush_uart();
            printf("\nInvalid word; Enter Mnemonic Word: ");
            get_serial_input(user_words[j], sizeof(user_words[j]));
        }
    }
    esp_log_level_set("*", CONFIG_LOG_DEFAULT_LEVEL);
    loading_disable();
    sodium_memzero(index, sizeof(index));

    // Join Mnemonic into single buffer
    CONFIDENTIAL char mnemonic[MNEMONIC_BUF_LEN];
    size_t offset=0;
    for(uint8_t i=0; i < sizeof(index); i++){
        strlcpy(mnemonic + offset, user_words[i], sizeof(mnemonic) - offset);
        offset += strlen(user_words[i]);
        mnemonic[offset++] = ' ';
    }
    mnemonic[offset - 1] = '\0'; //null-terminate, remove last space

    store_mnemonic_reboot(&menu, mnemonic);

    sodium_memzero(mnemonic, sizeof(mnemonic));
}
