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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu8g2.h"
#include "qrcode.h"
#include "qr.h"
#include "nano_lib.h"
#include "mbedtls/bignum.h"

void display_qr(menu8g2_t *menu, uint8_t x_offset, uint8_t y_offset,
        QRCode *qrcode, uint8_t scale){
    // Displays a fully formed QR Code to the OLED display
   
    MENU8G2_BEGIN_DRAW(menu)
        for (uint8_t y = 0; y < qrcode->size; y++) {
            for (uint8_t x = 0; x < qrcode->size; x++) {
                if(scale==1){
                    if (qrcode_getModule(qrcode, x, y)) {
                        u8g2_DrawPixel(menu->u8g2, x+x_offset, y+y_offset);
                    }
                }
                else{
                    if (qrcode_getModule(qrcode, x, y)) {
                        u8g2_DrawBox(menu->u8g2,
                                scale*x + x_offset,
                                scale*y + y_offset,
                                scale, scale);
                    }
                }
            }
        }
    MENU8G2_END_DRAW(menu)
}

void display_qr_center(menu8g2_t *menu, QRCode *qrcode, uint8_t scale){
    int16_t x_offset = (u8g2_GetDisplayWidth(menu->u8g2) - (qrcode->size * scale))/2;
    int16_t y_offset = (u8g2_GetDisplayHeight(menu->u8g2) - (qrcode->size * scale))/2;

    x_offset = (x_offset < 0) ? 0 : x_offset;
    y_offset = (y_offset < 0) ? 0 : y_offset;
    
    display_qr(menu, x_offset, y_offset, qrcode, scale);
}
