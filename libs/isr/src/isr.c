
void isr0()
{   
    __builtin_npu_write_control_reg(3, 11);
    for(;;){
	
	}
}
