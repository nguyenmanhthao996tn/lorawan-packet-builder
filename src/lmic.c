#include "lorawan_builder.h"

#if !defined(os_rlsbf4)
u4_t os_rlsbf4 (xref2cu1_t buf) {
    return (u4_t)((u4_t)buf[0] | ((u4_t)buf[1]<<8) | ((u4_t)buf[2]<<16) | ((u4_t)buf[3]<<24));
}
#endif

#if !defined(os_wlsbf4)
void os_wlsbf4 (xref2u1_t buf, u4_t v) {
    buf[0] = v;
    buf[1] = v>>8;
    buf[2] = v>>16;
    buf[3] = v>>24;
}
#endif

#if !defined(os_rmsbf4)
u4_t os_rmsbf4 (xref2cu1_t buf) {
    return (u4_t)((u4_t)buf[3] | ((u4_t)buf[2]<<8) | ((u4_t)buf[1]<<16) | ((u4_t)buf[0]<<24));
}
#endif

#if !defined(os_wmsbf4)
void os_wmsbf4 (xref2u1_t buf, u4_t v) {
    buf[3] = v;
    buf[2] = v>>8;
    buf[1] = v>>16;
    buf[0] = v>>24;
}
#endif

#if !defined(os_rlsbf2)
u2_t os_rlsbf2 (xref2cu1_t buf) {
    return (u2_t)((u2_t)buf[0] | ((u2_t)buf[1]<<8));
}
#endif

#if !defined(os_wlsbf2)
void os_wlsbf2 (xref2u1_t buf, u2_t v) {
    buf[0] = v;
    buf[1] = v>>8;
}
#endif

void micB0 (u4_t devaddr, u4_t seqno, int dndir, int len) {
  os_clearMem(AESaux, 16);
  AESaux[0]  = 0x49;
  AESaux[5]  = dndir ? 1 : 0;
  AESaux[15] = len;
  os_wlsbf4(AESaux + 6, devaddr);
  os_wlsbf4(AESaux + 10, seqno);
}

void aes_appendMic (xref2cu1_t key, u4_t devaddr, u4_t seqno, int dndir, xref2u1_t pdu, int len) {
  micB0(devaddr, seqno, dndir, len);
  os_copyMem(AESkey, key, 16);
  // MSB because of internal structure of AES
  os_wmsbf4(pdu + len, os_aes(AES_MIC, pdu, len));
}

void aes_cipher (xref2cu1_t key, u4_t devaddr, u4_t seqno, int dndir, xref2u1_t payload, int len) {
  if ( len <= 0 )
    return;
  os_clearMem(AESaux, 16);
  AESaux[0] = AESaux[15] = 1; // mode=cipher / dir=down / block counter=1
  AESaux[5] = dndir ? 1 : 0;
  os_wlsbf4(AESaux + 6, devaddr);
  os_wlsbf4(AESaux + 10, seqno);
  os_copyMem(AESkey, key, 16);
  os_aes(AES_CTR, payload, len);
}

uint8_t build_lorawan_packet(uint8_t *payload, uint8_t payload_len,
                                  uint16_t FCnt,
                                  uint8_t *target_buffer,
                                  uint8_t *NwkSKey, uint8_t *AppSKey, uint32_t devAddr, uint8_t FPort)
{
  os_clearMem(target_buffer, payload_len + 9 + 4);

  // MAC HEADER
  target_buffer[0] = 0x40;

  // Device Address
  target_buffer[1] = (devAddr >> 0) & 0xFF;
  target_buffer[2] = (devAddr >> 8) & 0xFF;
  target_buffer[3] = (devAddr >> 16) & 0xFF;
  target_buffer[4] = (devAddr >> 24) & 0xFF;

  // FCtrl
  target_buffer[5] = 0;

  // FCnt
  target_buffer[6] = (FCnt >> 0) & 0xFF;
  target_buffer[7] = (FCnt >> 8) & 0xFF;

  // FPort
  target_buffer[8] = FPort;

  // FRMPayload
  os_copyMem(target_buffer + 9, payload, payload_len);
  aes_cipher(AppSKey, devAddr, FCnt, 0x00 /* uplink */, target_buffer + 9, payload_len);

  // MIC
  aes_appendMic(NwkSKey, devAddr, FCnt, 0x00 /* uplink */, target_buffer, payload_len + 9);

  return (payload_len + 9 + 4);
}
