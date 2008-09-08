module host_sim(
  input             Reset,
  input             Clk_reg,
  output reg        CSB,
  output reg        WRB,
  output reg        CPU_init_end,
  output reg [15:0] CD_in,
  input      [15:0] CD_out,
  output reg  [7:0] CA
);

////////////////////////////////////////

task CPU_init;
  begin
    CA    = 0;
    CD_in = 0;
    WRB   = 1;
    CSB   = 1; 
  end
endtask

////////////////////////////////////////

task CPU_wr;
  input [6:0]  Addr;
  input [15:0] Data;
  begin
    CA    = {Addr,1'b0};
    CD_in = Data;
    WRB   = 0;
    CSB   = 0; 
    #20;
    CA    = 0;
    CD_in = 0;
    WRB   = 1;
    CSB   = 1;
    #20;
  end
endtask

/////////////////////////////////////////

task CPU_rd;
  input [6:0] Addr;
  begin
    CA  = {Addr,1'b0};
    WRB = 1;
    CSB = 0; 
    #20;
    CA  = 0;
    WRB = 1;
    CSB = 1;
    #20; 
  end
endtask

/////////////////////////////////////////

integer i;

reg [31:0] CPU_data [255:0];
reg [7:0]  write_times;
reg [7:0]  write_add;
reg [15:0] write_data;

initial
  begin
    CPU_init;
    CPU_init_end=0;
    //$readmemh("../data/CPU.vec",CPU_data);
    //{write_times,write_add,write_data}=CPU_data[0];
    {write_times,write_add,write_data}='b0;
    #90;
    for (i=0;i<write_times;i=i+1)
      begin
        {write_times,write_add,write_data}=CPU_data[i];
        CPU_wr(write_add[6:0],write_data);
      end
    CPU_init_end=1;
  end
endmodule
