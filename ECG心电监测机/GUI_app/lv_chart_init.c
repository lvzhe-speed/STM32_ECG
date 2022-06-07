#include "lv_chart_init.h"
#include "lvgl.h"
#include "ecg_data_process.h"
#include "usart.h"
#include "bsp_ads1292.h"
#include "ecg_heart_rate.h"
#include "ui.h"
#include "lmt70.h"
#include "ecg_lpf_filter.h"

chart_type chart= {0};
#define TDIV        19
#define POINT_COUNT   (30*(TDIV+1))  //每条数据线所具有的数据点个数
#define MAX_YPOS   400

System_State_dev state_pcb={
.SampleStartFlag=false,
.ChartShowStartFlag=false,
.StartSmoothFilterFlag=true
};


lv_style_t roller_sel_style,roller_bg_style;
lv_style_t Top_Content_style;
lv_style_t  Panel_Content_label_style;
lv_style_t  TEMPNum_style;//数字字体
lv_style_t  QRSNum_style;//数字字体
lv_style_t  HRNum_style;//数字字体
lv_style_t chart_style;//图表样式
lv_style_t ui_BTN_Start_style;

lv_obj_t * scr;     //屏幕对象
lv_obj_t * roller; //滚轮对象

lv_obj_t * ui_Top_Content;//ui顶部
lv_obj_t * menu_btn;  //菜单标签对象
lv_obj_t * back_btn; //返回标签对象
lv_obj_t * Right_Lead_state_led;
lv_obj_t * Left_Lead_state_led;



lv_obj_t * ui_Panel_Content;
lv_obj_t * ui_IMG_Line_1;
lv_obj_t * ui_IMG_Line_2;
lv_obj_t * ui_IMG_Line_3;
lv_obj_t * ui_Number_TEMP;
lv_obj_t * ui_Number_QRS;
lv_obj_t * ui_Number_HR;
lv_obj_t * ui_Label_TEMP;
lv_obj_t * ui_Label_Centigrade;
lv_obj_t * ui_Label_QRS;
lv_obj_t * ui_Label_Width;
lv_obj_t * ui_Label_HR;
lv_obj_t * ui_IMG_Heart;
lv_obj_t * chart1;  //图表对象
lv_chart_series_t * series1;
lv_obj_t * ui_IMG_BTN_Bg;
lv_obj_t * ui_BTN_Start;

static void menu_test(void);
static void roller_event_handler(lv_obj_t * obj, lv_event_t event);
static void menu_event_handler(lv_obj_t * obj, lv_event_t event);
static void back_event_handler(lv_obj_t * obj, lv_event_t event);
static int Transf_EcgData_To_Vert(int data,u16 scal);
static void ShowHrTask_cb(lv_task_t* task);
static void ShowLeadTask_cb(lv_task_t* task);
static void Change_Graph_Vert_cb(lv_task_t* task);
static void SetSystemState_cb(lv_task_t* task);
static void HeartAnim_Animation(lv_obj_t * TargetObject, int delay);
/*****菜单的执行函数*****/

void Set_chart_div_line(void)
{
    static u8 i=0;
    if(i==0)
    {
        chart.tdiv=TDIV;
        chart.vdiv=9;
        i++;
    } else if(i==1)
    {
        chart.tdiv=0;
        chart.vdiv=0;
        i=0;
    }
    lv_chart_set_div_line_count(chart1,chart.vdiv,chart.tdiv);//设置水平和垂直分割线
}


void send_type_server(void)
{
    static u8 i=0;
    if(i==0)
    {
        ecg_graph.send_type=GRAPH;
        i++;
    } else if(i==1)
    {
        ecg_graph.send_type=USART;
        i=0;
    }


}


void clear_step(void)
{
	lv_chart_clear_series(chart1,series1);
	  lv_chart_refresh(chart1);
	hr.rate=0.0;
	hr.QRS=0;
}

void smooth_filter(void)
{

    static u8 i=0;
    if(i==0)
    {
        state_pcb.StartSmoothFilterFlag=false;
        i++;
    } else if(i==1)
    {
        state_pcb.StartSmoothFilterFlag=true;
        i=0;
    }




}

void (*oper_fuc[4])();//函数指针数组

void Menuitem_Init(void)
{
    oper_fuc[0]=send_type_server;
    oper_fuc[1]=Set_chart_div_line;
    oper_fuc[2]=clear_step;
	oper_fuc[3]=smooth_filter;
}


static void menu_test(void)
{
//2.创建滚轮对象

    roller = lv_roller_create(scr,NULL);
    lv_roller_set_options(roller,"send_type\nset_div_line\nclear_step\nsmooth_filter",LV_ROLLER_MODE_NORMAL);//设置所有的选项值,循环滚动模式
    lv_roller_set_selected(roller,2,LV_ANIM_ON);//设置默认选中的值
    lv_obj_add_style(roller, LV_ROLLER_PART_BG, &roller_bg_style);
    lv_obj_add_style(roller, LV_ROLLER_PART_SELECTED, &roller_sel_style);
    lv_roller_set_anim_time(roller,600);
    lv_roller_set_fix_width(roller,300);
    lv_roller_set_visible_row_count(roller,4);//设置可见的行数
    lv_roller_set_align(roller,LV_ALIGN_IN_TOP_LEFT);
    lv_obj_align(roller,NULL,LV_ALIGN_IN_TOP_LEFT,0,0);//设置与屏幕居中对齐
    lv_obj_set_event_cb(roller,roller_event_handler);//注册事件回调函数
}

/******返回回调函数*******/
static void BTN_Start_event_handler(lv_obj_t * obj, lv_event_t event)
{
	
	if(lv_btn_get_state(obj)==LV_BTN_STATE_CHECKED_RELEASED)
    {
			state_pcb.ChartShowStartFlag=true;
    }		
	else if(lv_btn_get_state(obj)==LV_BTN_STATE_RELEASED){
		state_pcb.ChartShowStartFlag=false;
	}


}


static void back_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if((obj==back_btn)&&(event==LV_EVENT_CLICKED))
    {
        lv_obj_set_click(back_btn,false);//使能点击功能
        if(roller!=NULL) {
            lv_obj_del(roller);
            roller=NULL;
        }
    }
}
//事件处理
static void menu_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(obj==menu_btn)
    {
        if(event==LV_EVENT_RELEASED)
        {
            lv_obj_set_click(back_btn,true);//使能点击功能
            lv_obj_set_event_cb(back_btn,back_event_handler);//设置事件回调函数
            menu_test();
        }
    }
}

static void roller_event_handler(lv_obj_t * obj, lv_event_t event)
{
    static unsigned char count=0;

    if(event==LV_EVENT_VALUE_CHANGED)
    {
        count=lv_roller_get_selected(obj);
    }
    if(event==LV_EVENT_CLICKED)
    {
        oper_fuc[count]();
    }

}



void _ui_anim_callback_set_image_zoom(void *a, short v)
{
    lv_img_set_zoom((lv_obj_t *)a, v);
}



static void HeartAnim_Animation(lv_obj_t * TargetObject, int delay)
{

    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 200);
	lv_anim_set_var(&PropertyAnimation_0,TargetObject);

    lv_anim_set_exec_cb(&PropertyAnimation_0, (lv_anim_exec_xcb_t)_ui_anim_callback_set_image_zoom); //渐变函数(&PropertyAnimation_0, _ui_anim_callback_set_image_zoom);
    lv_anim_set_values(&PropertyAnimation_0, 400, 256);
	lv_anim_path_set_cb(&PropertyAnimation_0.path,lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
	
    lv_anim_start(&PropertyAnimation_0);

}

/**
  * @Brief 绘制整个界面框架
  * @Call
  * @Param  None
  * @Note
  * @Retval None
  */
void lv_chart_init(void)
{
    lv_obj_t * menu_label;
    lv_obj_t * back_label;
    scr = lv_scr_act();//获取当前活跃的屏幕对象

	lv_style_init(&Top_Content_style);
	lv_style_set_bg_color(&Top_Content_style, LV_STATE_DEFAULT,  lv_color_hex(0xC8C8C8));
    lv_style_set_bg_opa(&Top_Content_style, LV_STATE_DEFAULT,  255);
	lv_style_set_text_font(&Top_Content_style, LV_STATE_DEFAULT, &lv_font_montserrat_18);
	lv_style_set_text_color(&Top_Content_style,LV_STATE_DEFAULT,lv_color_hex(0xFFFFFF));
	
	
	// 创建选择项被选中时的样式
    lv_style_init(&roller_sel_style);
    lv_style_init(&roller_bg_style);
    lv_style_set_text_line_space(&roller_bg_style,LV_STATE_DEFAULT,30);
    lv_style_set_text_font(&roller_bg_style,LV_STATE_DEFAULT,&lv_font_montserrat_18);
    lv_style_set_bg_color(&roller_bg_style, LV_STATE_DEFAULT,  LV_COLOR_WHITE);/*设置上部分背景色*/
    lv_style_set_bg_grad_color(&roller_bg_style, LV_STATE_DEFAULT,  LV_COLOR_WHITE);/*设置下部分背景色*/
    lv_style_set_text_color(&roller_bg_style,LV_STATE_DEFAULT,LV_COLOR_BLACK);
    lv_style_set_bg_color(&roller_sel_style, LV_STATE_DEFAULT,  LV_COLOR_RED);/*设置上部分背景色*/
    lv_style_set_bg_grad_color(&roller_sel_style, LV_STATE_DEFAULT,  LV_COLOR_RED);/*设置下部分背景色*/
    lv_style_set_text_color(&roller_sel_style,LV_STATE_DEFAULT,LV_COLOR_WHITE);
	
	ui_Top_Content = lv_obj_create(scr,NULL);
    lv_obj_set_height(ui_Top_Content, 60);
    lv_obj_set_width(ui_Top_Content, 800);
    lv_obj_set_x(ui_Top_Content, 0);
    lv_obj_set_y(ui_Top_Content, 0);
	lv_obj_add_style(ui_Top_Content,LV_LABEL_PART_MAIN,&Top_Content_style);
	
	menu_btn = lv_btn_create(ui_Top_Content, NULL);
    lv_obj_set_size(menu_btn,100,50);//设置大小
    menu_label = lv_label_create(menu_btn,NULL);//给btn2添加label子对象
    lv_obj_add_style(menu_label,LV_LABEL_PART_MAIN, &Top_Content_style);
    lv_obj_add_style(menu_btn,LV_BTN_PART_MAIN,&Top_Content_style);
    lv_label_set_text_static(menu_label, LV_SYMBOL_HOME " Menu");
    lv_obj_align(menu_btn,ui_Top_Content,LV_ALIGN_IN_TOP_LEFT,5,5);
    lv_btn_set_layout(menu_btn,LV_LAYOUT_CENTER);//设置按钮2的布局方式,使label处于正中间,当然了如果不设置的话,默认也是正中间的
    lv_obj_set_click(menu_btn,true);
    lv_obj_set_event_cb(menu_btn,menu_event_handler);//设置事件回调函数


    back_btn = lv_btn_create(ui_Top_Content, NULL);
    lv_obj_set_size(back_btn,100,50);//设置大小
    back_label = lv_label_create(back_btn,NULL);//给btn2添加label子对象
    lv_obj_add_style(back_label,LV_LABEL_PART_MAIN, &Top_Content_style);
    lv_obj_add_style(back_btn,LV_BTN_PART_MAIN,&Top_Content_style);
    lv_label_set_text_static(back_label, LV_SYMBOL_BACKSPACE " Back");
    lv_obj_align(back_btn,ui_Top_Content,LV_ALIGN_IN_TOP_RIGHT,5,5);
    lv_btn_set_layout(back_btn,LV_LAYOUT_CENTER);//设置按钮2的布局方式,使label处于正中间,当然了如果不设置的话,默认也是正中间的
    lv_obj_set_click(back_btn,false);
   
   
	Right_Lead_state_led=lv_led_create(ui_Top_Content,NULL);
	lv_obj_set_size(Right_Lead_state_led,20,20);
	lv_obj_align(Right_Lead_state_led, ui_Top_Content, LV_ALIGN_CENTER, 20, 0);
    lv_led_off(Right_Lead_state_led);
	
	Left_Lead_state_led=lv_led_create(ui_Top_Content,NULL);
	lv_obj_set_size(Left_Lead_state_led,20,20);
	lv_obj_align(Left_Lead_state_led, ui_Top_Content, LV_ALIGN_CENTER, -20, 0);
    lv_led_off(Left_Lead_state_led);
	//2.创建图表对象
    //2.1 创建图表对象
	
	// ui_Panel_Content
    ui_Panel_Content = lv_obj_create(scr,NULL);
    lv_obj_set_height(ui_Panel_Content, 420);
    lv_obj_set_width(ui_Panel_Content, 800);
    lv_obj_set_x(ui_Panel_Content, 0);
    lv_obj_set_y(ui_Panel_Content, 60);

    // ui_IMG_Line_1
    ui_IMG_Line_1 = lv_img_create(ui_Panel_Content,NULL);
    lv_img_set_src(ui_IMG_Line_1, &ui_img_img_line_png);
    lv_obj_set_width(ui_IMG_Line_1, 200);
    lv_obj_set_height(ui_IMG_Line_1, 4);
    lv_obj_align(ui_IMG_Line_1,ui_Panel_Content, LV_ALIGN_IN_TOP_RIGHT,0,100);


    // ui_IMG_Line_2
    ui_IMG_Line_2 = lv_img_create(ui_Panel_Content,NULL);
    lv_img_set_src(ui_IMG_Line_2, &ui_img_img_line_png);
    lv_obj_set_width(ui_IMG_Line_2, 200);
    lv_obj_set_height(ui_IMG_Line_2, 4);
    lv_obj_align(ui_IMG_Line_2, ui_Panel_Content,LV_ALIGN_IN_TOP_RIGHT,0,200);

    // ui_IMG_Line_3

    ui_IMG_Line_3 = lv_img_create(ui_Panel_Content,NULL);
    lv_img_set_src(ui_IMG_Line_3, &ui_img_img_line_png);
    lv_obj_set_width(ui_IMG_Line_3, 200);
    lv_obj_set_height(ui_IMG_Line_3, 4);
    lv_obj_align(ui_IMG_Line_3, ui_Panel_Content,LV_ALIGN_IN_TOP_RIGHT,0,300);
    // ui_Number_TEMP

    lv_style_init(&TEMPNum_style);
    lv_style_set_text_font(&TEMPNum_style, LV_STATE_DEFAULT, &ui_font_Big_Number);
	lv_style_set_text_color(&TEMPNum_style,LV_STATE_DEFAULT,lv_color_hex(0x1052F5));
    ui_Number_TEMP = lv_label_create(ui_Panel_Content,NULL);
    lv_obj_align(ui_Number_TEMP,ui_Panel_Content,LV_ALIGN_IN_TOP_MID,230,20);
    lv_label_set_text(ui_Number_TEMP, "32");
	lv_obj_add_style(ui_Number_TEMP,LV_LABEL_PART_MAIN, &TEMPNum_style);

    // ui_Number_QRS
	lv_style_init(&QRSNum_style);
    lv_style_set_text_font(&QRSNum_style, LV_STATE_DEFAULT, &ui_font_Big_Number);
	lv_style_set_text_color(&QRSNum_style,LV_STATE_DEFAULT,lv_color_hex(0x10F557));
    ui_Number_QRS = lv_label_create(ui_Panel_Content,NULL);
    lv_obj_align(ui_Number_QRS,ui_Panel_Content, LV_ALIGN_IN_TOP_MID,230,120);
    lv_label_set_text(ui_Number_QRS, "65");
	lv_obj_add_style(ui_Number_QRS,LV_LABEL_PART_MAIN, &QRSNum_style);

    // ui_Number_HR
	lv_style_init(&HRNum_style);
    lv_style_set_text_font(&HRNum_style, LV_STATE_DEFAULT, &ui_font_Big_Number);
	lv_style_set_text_color(&HRNum_style,LV_STATE_DEFAULT,lv_color_hex(0xF51020));
    ui_Number_HR = lv_label_create(ui_Panel_Content,NULL);
    lv_obj_align(ui_Number_HR,ui_Panel_Content, LV_ALIGN_IN_TOP_MID,230,220);
    lv_label_set_text(ui_Number_HR, "72");
	lv_obj_add_style(ui_Number_HR,LV_LABEL_PART_MAIN, &HRNum_style);
	
	
	
	lv_style_init(&Panel_Content_label_style);
	lv_style_set_text_font(&Panel_Content_label_style, LV_STATE_DEFAULT, &lv_font_montserrat_18);
    // ui_Label_TEMP
    ui_Label_TEMP = lv_label_create(ui_Panel_Content,NULL);
	lv_obj_add_style(ui_Label_TEMP,LV_LABEL_PART_MAIN, &Panel_Content_label_style);
    lv_obj_align(ui_Label_TEMP, ui_Panel_Content,LV_ALIGN_IN_TOP_MID,350,20);
    lv_label_set_text(ui_Label_TEMP, "TEMP");
    // ui_Label_Centigrade
    ui_Label_Centigrade = lv_label_create(ui_Panel_Content,NULL);
    lv_obj_align(ui_Label_Centigrade,ui_Panel_Content, LV_ALIGN_IN_TOP_MID,350,60);
    lv_label_set_text(ui_Label_Centigrade, "%c");


    // ui_Label_QRS
    ui_Label_QRS = lv_label_create(ui_Panel_Content,NULL);
		lv_obj_add_style(ui_Label_QRS,LV_LABEL_PART_MAIN, &Panel_Content_label_style);
    lv_obj_align(ui_Label_QRS,ui_Panel_Content, LV_ALIGN_IN_TOP_MID,350,120);
    lv_label_set_text(ui_Label_QRS, "QRS");
    // ui_Label_Width
    ui_Label_Width = lv_label_create(ui_Panel_Content,NULL);
    lv_obj_align(ui_Label_Width,ui_Panel_Content, LV_ALIGN_IN_TOP_MID,350,160);
    lv_label_set_text(ui_Label_Width, "ms");


    // ui_Label_HR

    ui_Label_HR = lv_label_create(ui_Panel_Content,NULL);
	lv_obj_add_style(ui_Label_HR,LV_LABEL_PART_MAIN, &Panel_Content_label_style);
    lv_obj_align(ui_Label_HR,ui_Panel_Content ,LV_ALIGN_IN_TOP_MID,350,220);
    lv_label_set_text(ui_Label_HR, "HR");
    // ui_IMG_Heart
    ui_IMG_Heart = lv_img_create(ui_Panel_Content,NULL);
    lv_img_set_src(ui_IMG_Heart, &ui_img_img_heart_big_png);
    lv_obj_align(ui_IMG_Heart, ui_Panel_Content,LV_ALIGN_IN_TOP_MID,350,260);
	
	
    chart1 = lv_chart_create(ui_Panel_Content,NULL);
    lv_obj_set_size(chart1,600,400);//设置图表的大小
    lv_obj_align(chart1,NULL,LV_ALIGN_IN_TOP_LEFT,0,10);//设置对齐方式
    lv_chart_set_type(chart1,LV_CHART_TYPE_LINE);//设置为散点和折线的组合

    lv_style_init(&chart_style);
    lv_style_set_bg_color(&chart_style, LV_STATE_DEFAULT,  LV_COLOR_MAKE(0x39, 0x3C, 0x4A));
    lv_style_set_line_width(&chart_style,LV_STATE_DEFAULT,3);
    lv_style_set_size(&chart_style,LV_STATE_DEFAULT,0);
    lv_obj_add_style(chart1, LV_CHART_PART_BG, &chart_style);
    lv_obj_add_style(chart1, LV_CHART_PART_SERIES, &chart_style);
    lv_chart_set_point_count(chart1,POINT_COUNT);//设置每条数据线所具有的数据点个数,如果不设置的话,则默认值是10
    lv_chart_set_div_line_count(chart1,chart.vdiv,chart.tdiv);//设置水平和垂直分割线
    lv_chart_set_range(chart1,ecg_graph.y_min,ecg_graph.y_min+MAX_YPOS);//设置y轴的数值范围,[0,100]也是默认值
    lv_chart_set_update_mode(chart1,LV_CHART_UPDATE_MODE_CIRCULAR);
    series1 = lv_chart_add_series(chart1,LV_COLOR_YELLOW);//指定为红色

    lv_obj_set_state(chart1,LV_STATE_DEFAULT);
    lv_obj_set_click(chart1,DISABLE);
	
	
	    // ui_IMG_BTN_Bg

    ui_IMG_BTN_Bg = lv_img_create(ui_Panel_Content,NULL);
    lv_img_set_src(ui_IMG_BTN_Bg, &ui_img_img_btn_bg_png);
    lv_obj_align(ui_IMG_BTN_Bg, ui_Panel_Content,LV_ALIGN_IN_TOP_MID,300,310);
	lv_obj_set_state(ui_IMG_BTN_Bg,LV_STATE_DISABLED);


    // ui_BTN_Power

    ui_BTN_Start = lv_imgbtn_create(ui_IMG_BTN_Bg,NULL);
	lv_style_init(&ui_BTN_Start_style);
    lv_obj_set_width(ui_BTN_Start, 90);
    lv_obj_set_height(ui_BTN_Start, 90);

	
    lv_obj_align(ui_BTN_Start, ui_IMG_BTN_Bg,LV_ALIGN_CENTER,0,0);
    lv_style_set_radius(&ui_BTN_Start_style, 90, LV_STATE_DEFAULT);
    lv_style_set_bg_main_stop(&ui_BTN_Start_style, 0, LV_STATE_DEFAULT);
    lv_style_set_bg_grad_stop(&ui_BTN_Start_style, 255, LV_STATE_DEFAULT);
    lv_imgbtn_set_src(ui_BTN_Start, LV_BTN_STATE_RELEASED, &ui_img_img_btn_off_png);
	lv_imgbtn_set_src(ui_BTN_Start, LV_BTN_STATE_PRESSED, &ui_img_img_btn_off_png);
    lv_style_set_border_width(&ui_BTN_Start_style, 0, LV_STATE_DEFAULT);
	lv_imgbtn_set_src(ui_BTN_Start, LV_BTN_STATE_CHECKED_RELEASED,&ui_img_img_btn_on_png);
	lv_imgbtn_set_src(ui_BTN_Start, LV_BTN_STATE_CHECKED_PRESSED,&ui_img_img_btn_on_png);
    lv_style_set_shadow_color(&ui_BTN_Start_style, LV_STATE_CHECKED,lv_color_hex(0x00A1FF));
    lv_style_set_shadow_opa(&ui_BTN_Start_style, LV_STATE_CHECKED,255);
	lv_imgbtn_set_checkable(ui_BTN_Start, true);

	
	lv_obj_add_style(ui_BTN_Start,LV_OBJ_PART_MAIN, &ui_BTN_Start_style);
	  lv_obj_set_event_cb(ui_BTN_Start, BTN_Start_event_handler);


    Menuitem_Init();
	lv_task_create(ShowHrTask_cb,2000,LV_TASK_PRIO_MID,0);
	lv_task_create(ShowLeadTask_cb,1000,LV_TASK_PRIO_LOW,0);
	lv_task_create(Change_Graph_Vert_cb,300,LV_TASK_PRIO_HIGH,0);
	lv_task_create(SetSystemState_cb,800,LV_TASK_PRIO_LOWEST,0);
}

static void SetSystemState_cb(lv_task_t* task)
{
	if(ecg_info.ecg_state==ECG_ON && state_pcb.ChartShowStartFlag==true){
		state_pcb.SampleStartFlag=true;
	}else{
		state_pcb.SampleStartFlag=false;
	}
}
	
static void ShowHrTask_cb(lv_task_t* task)
{
	char tbuf[4],hbuf[4],qbuf[4];
	if(ecg_info.ecg_state==ECG_ON){
	HeartAnim_Animation(ui_IMG_Heart, 0);
	}
	sprintf((char*)qbuf,"%d",(int)hr.QRS);
	lv_label_set_text(ui_Number_QRS,qbuf);
	
	sprintf((char*)hbuf,"%d",(int)hr.rate);
	lv_label_set_text(ui_Number_HR,hbuf);	
	
	sprintf((char*)tbuf,"%d",(int)lmt70_get_temperature());
	lv_label_set_text(ui_Number_TEMP,tbuf);
}


static void ShowLeadTask_cb(lv_task_t* task)
{
	if(ecg_info.left_lead_wire_state==LEAD_WIRE_ON){
		lv_led_on(Left_Lead_state_led);
	}else {
        lv_led_off(Left_Lead_state_led);
    }
	
	if(ecg_info.right_lead_wrie_state==LEAD_WIRE_ON){
		lv_led_on(Right_Lead_state_led);
	}else {
        lv_led_off(Right_Lead_state_led);
    }
}


int change=0;
static void Change_Graph_Vert_cb(lv_task_t* task)
{
	if(change<ecg_graph.y_min) {
        ecg_graph.y_min=change-40;
        lv_chart_set_y_range(chart1,LV_CHART_AXIS_PRIMARY_Y,ecg_graph.y_min,ecg_graph.y_min+MAX_YPOS);//设置y轴的数值范围,[0,100]也是默认值
    }
    else if(change>ecg_graph.y_min+MAX_YPOS)
        ecg_graph.y_min=change-40;
    lv_chart_set_y_range(chart1,LV_CHART_AXIS_PRIMARY_Y,ecg_graph.y_min,ecg_graph.y_min+MAX_YPOS);//设置y轴的数值范围,[0,100]也是默认值
}
/**
  * @Brief 将ECG数据转换成纵坐标数值
  * @Call
  * @Param  level:零点位置，scal:刻度
  * @Note
  * @Retval None
  */
static int Transf_EcgData_To_Vert(int data,u16 scal)
{
    data=data/scal;
	if(state_pcb.StartSmoothFilterFlag==true)
		data=(int)alg(data);
    change=data;
    return data;
}

static void chart_add_data(lv_coord_t data)
{
	uint16_t i=1;
    lv_chart_set_next(chart1, series1, data);
    uint16_t p = lv_chart_get_point_count(chart1);
    uint16_t s = lv_chart_get_x_start_point(series1);
	
    lv_coord_t * a = series1->points;
	for(i=1;i<=10;i++){
		a[(s + i) % p] = LV_CHART_POINT_DEF;
	}

}

void Wave_show(void)
{
    int value=0;
    if(ReadEcgOutBuffer(&value)!=0) {
        if(ecg_graph.send_type==GRAPH) {
            ecg_graph.y_pose=Transf_EcgData_To_Vert(value,ecg_graph.sacle);
			  chart_add_data(ecg_graph.y_pose);
            set_data_into_heart_buff(ecg_graph.y_pose);
        } else if(ecg_graph.send_type==USART) {
            //EcgSendByUart(value);
			printf("%d\r\n",(int)alg(value/200));
        }
    }
}



//定时器3中断服务程序
void TIM3_IRQHandler(void)
{ 	static u8 show_cnt=0;   		  			    
	if(TIM3->SR&TIM_IT_Update)//溢出中断
	{	show_cnt++;
		lv_tick_inc(1);//lvgl的1ms心跳
		if(show_cnt==3){
		show_cnt=0;
		 Wave_show();
		}
	}				   
	TIM3->SR = (uint16_t)~TIM_IT_Update;
}

