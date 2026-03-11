#ifdef __CCE_KT_TEST__
#define __aicore__ 
#else
#define __aicore__ [aicore]
#endif

extern "C" inline __aicore__ void vector_random_buff_kernel0() {
set_vector_mask((uint64_t)-1, (uint64_t)-1);
set_atomic_none();
   half reg_buf5[1] = {0};
     int8_t reg_buf6[1] = {0};
  __ubuf__   uint8_t* src_ub = (__ubuf__  uint8_t *)get_imm(0);
  // "aicore arch: Ascend910"
  reg_buf5[0] = (half)5.907702e+00f;
  reg_buf6[0] = (int8_t)6;
  set_vector_mask(0x0, 0xffff);
  for (int32_t i = 0; i < 8; ++i) {
    vadds(((__ubuf__ half *)src_ub + ((i * 16384) + (((int32_t)reg_buf6[0]) * 16))), ((__ubuf__ half *)src_ub + ((i * 16384) + (((int32_t)reg_buf6[0]) * 16))), reg_buf5[0], 96, 1, 1, 8, 8);
  }
  pipe_barrier(PIPE_ALL);
  
  uint64_t status_overflow[1] = {0};
  status_overflow[0] = get_status();
  status_overflow[0] = (status_overflow[0] << 32) >> 32;
  uint64_t status_mask = 0x520;
  status_overflow[0] = status_overflow[0] & status_mask;
  if (status_overflow[0]) {
    uint64_t *ptr = (uint64_t *)get_imm(0x43FE0);
    uint64_t buff[4];
    buff[0] = ptr[0];
    buff[1] = ptr[1];
    buff[2] = ptr[2] | status_overflow[0];
    buff[3] = ptr[3];
    
    if (buff[0] == 0) {
      ptr[0] = 0xFFFFFFFFFFFFFFFF;
      ptr[1] = block_idx;
    }
    ptr[2] = buff[2];
    
    pipe_barrier(PIPE_ALL);
  }
  
  pipe_barrier(PIPE_ALL);

}

