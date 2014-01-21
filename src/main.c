/*

   Digital watch based on my old Diesel watch

   A digital watch which makes use of the whole of the pebble screen
   with large digits, seconds and date.  AM/PM version.
   
   - author: Neil Whitaker / neil@hencam.co.uk / http://www.hencam.co.uk

*/

#include <pebble.h>

	
#define NUMBER_OF_IMAGES 44

// images used by the face
const int IMAGE_RESOURCE_IDS[NUMBER_OF_IMAGES] = {
	RESOURCE_ID_IMAGE_AM,
	RESOURCE_ID_IMAGE_PM,
	RESOURCE_ID_IMAGE_BIG0,
	RESOURCE_ID_IMAGE_BIG1,
	RESOURCE_ID_IMAGE_BIG2,
	RESOURCE_ID_IMAGE_BIG3,
	RESOURCE_ID_IMAGE_BIG4,
	RESOURCE_ID_IMAGE_BIG5,
	RESOURCE_ID_IMAGE_BIG6,
	RESOURCE_ID_IMAGE_BIG7,
	RESOURCE_ID_IMAGE_BIG8,
	RESOURCE_ID_IMAGE_BIG9,
	RESOURCE_ID_IMAGE_BIGSPACER,
	RESOURCE_ID_IMAGE_BIGTHIN1,
	RESOURCE_ID_IMAGE_DAYSUN,
	RESOURCE_ID_IMAGE_DAYMON,
	RESOURCE_ID_IMAGE_DAYTUE,
	RESOURCE_ID_IMAGE_DAYWED,
	RESOURCE_ID_IMAGE_DAYTHU,
	RESOURCE_ID_IMAGE_DAYFRI,
	RESOURCE_ID_IMAGE_DAYSAT,
	RESOURCE_ID_IMAGE_SMALL0,
	RESOURCE_ID_IMAGE_SMALL1,
	RESOURCE_ID_IMAGE_SMALL2,
	RESOURCE_ID_IMAGE_SMALL3,
	RESOURCE_ID_IMAGE_SMALL4,
	RESOURCE_ID_IMAGE_SMALL5,
	RESOURCE_ID_IMAGE_SMALL6,
	RESOURCE_ID_IMAGE_SMALL7,
	RESOURCE_ID_IMAGE_SMALL8,
	RESOURCE_ID_IMAGE_SMALL9,
	RESOURCE_ID_IMAGE_SMALLREV0,
	RESOURCE_ID_IMAGE_SMALLREV1,
	RESOURCE_ID_IMAGE_SMALLREV2,
	RESOURCE_ID_IMAGE_SMALLREV3,
	RESOURCE_ID_IMAGE_SMALLREV4,
	RESOURCE_ID_IMAGE_SMALLREV5,
	RESOURCE_ID_IMAGE_SMALLREV6,
	RESOURCE_ID_IMAGE_SMALLREV7,
	RESOURCE_ID_IMAGE_SMALLREV8,
	RESOURCE_ID_IMAGE_SMALLREV9,
	RESOURCE_ID_IMAGE_SMALLREVSPACER,
	RESOURCE_ID_IMAGE_TOPBAR,
	RESOURCE_ID_IMAGE_BLANKPIXEL
};


//create a bitmap holder for each image
static GBitmap *images[NUMBER_OF_IMAGES];



// our window
static Window *window;

// the different 'slots' or layers the face uses
# define month_tens_layer 0
# define month_ones_layer 1
# define month_seperator_layer 2
# define day_tens_layer 3
# define day_ones_layer 4
# define day_text_layer 5
# define secs_tens_layer 6
# define secs_ones_layer 7
# define ampm_layer 8
# define hours_tens_layer 9
# define hours_ones_layer 10
# define time_seperator_layer 11
# define mins_tens_layer 12
# define mins_ones_layer 13
# define topbar_layer 14

static BitmapLayer* dlayers[15];


// handle updating the required items on the face - only update those that have changed!
static void handle_second_tick(struct tm* t, int mode)
{
	int tens = t->tm_sec / 10 % 10;
	int ones = t->tm_sec % 10;
	
	bitmap_layer_set_bitmap(dlayers[secs_tens_layer], images[tens + 21]);
	bitmap_layer_set_bitmap(dlayers[secs_ones_layer], images[ones + 21]);
	
	if ((t->tm_sec == 0) || (mode == 1))
	{
		int tens = t->tm_min / 10 % 10;
		int ones = t->tm_min % 10;
		
		bitmap_layer_set_bitmap(dlayers[mins_tens_layer], images[tens + 2]);
		bitmap_layer_set_bitmap(dlayers[mins_ones_layer], images[ones + 2]);
	}
	
	if ((t->tm_min == 0) || (mode == 1))
	{
		int oval = t->tm_hour;
		int val = oval;
		if (val > 12)
		{
			val -= 12;
		}
		
		if (mode == 1)
		{
			if (oval < 12)
			{
				bitmap_layer_set_bitmap(dlayers[ampm_layer], images[0]);
			}
			else
			{
				bitmap_layer_set_bitmap(dlayers[ampm_layer], images[1]);
			}
		}
		else
		{
			if (oval == 0)
			{
				bitmap_layer_set_bitmap(dlayers[ampm_layer], images[0]);
			}
			else if (oval == 12)
			{
				bitmap_layer_set_bitmap(dlayers[ampm_layer], images[1]);
			}

		}
		
		int tens = val / 10 % 10;
		int ones = val % 10;
		
		if (tens == 0)
		{
			bitmap_layer_set_bitmap(dlayers[hours_tens_layer], images[43]);
		}
		else if ((tens == 12) || (mode == 1))
		{
			bitmap_layer_set_bitmap(dlayers[hours_tens_layer], images[13]);
		}
		
		bitmap_layer_set_bitmap(dlayers[hours_ones_layer], images[ones + 2]);
	}
	
	if ((t->tm_hour == 0) || (mode == 1))
	{
		int tens = t->tm_mday / 10 % 10;
		int ones = t->tm_mday % 10;
		
		bitmap_layer_set_bitmap(dlayers[day_tens_layer], images[tens + 31]);
		bitmap_layer_set_bitmap(dlayers[day_ones_layer], images[ones + 31]);
		
		bitmap_layer_set_bitmap(dlayers[day_text_layer], images[t->tm_wday + 14]);
	}
	
	if ((t->tm_mday == 1) || (mode == 1))
	{
		int tens = t->tm_mon / 10 % 10;
		int ones = t->tm_mon % 10;
		
		bitmap_layer_set_bitmap(dlayers[month_tens_layer], images[tens + 31]);
		bitmap_layer_set_bitmap(dlayers[month_ones_layer], images[ones + 31]);
	}
}





// this is called by the subscribed service every second, and calls the update handler with mode 0 (normal) not 1 (init)
static void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
	handle_second_tick(tick_time, 0);
}


// init the face - add the windo then layers for each item on the face, lastly call the init to place the time on screen then subscribe to udpate the face every second
void handle_init(void)
{
	window = window_create();
	Layer *window_layer = window_get_root_layer(window);
	
	dlayers[topbar_layer]			= bitmap_layer_create(GRect(0, 0, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[topbar_layer]));
	
	dlayers[month_tens_layer]		= bitmap_layer_create(GRect(0, 0, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[month_tens_layer]));
	
	dlayers[month_ones_layer]		= bitmap_layer_create(GRect(30, 0, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[month_ones_layer]));
	
	dlayers[month_seperator_layer]	= bitmap_layer_create(GRect(55, 0, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[month_seperator_layer]));
	
	dlayers[day_tens_layer]			= bitmap_layer_create(GRect(75, 0, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[day_tens_layer]));
	
	dlayers[day_ones_layer]			= bitmap_layer_create(GRect(105, 0, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[day_ones_layer]));
	
	dlayers[day_text_layer]			= bitmap_layer_create(GRect(0, 40, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[day_text_layer]));
	
	dlayers[secs_tens_layer]		= bitmap_layer_create(GRect(90, 40, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[secs_tens_layer]));
	
	dlayers[secs_ones_layer]		= bitmap_layer_create(GRect(120, 40, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[secs_ones_layer]));
	
	dlayers[ampm_layer]				= bitmap_layer_create(GRect(0, 95, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[ampm_layer]));
	
	dlayers[hours_tens_layer]		= bitmap_layer_create(GRect(20, 95, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[hours_tens_layer]));
	
	dlayers[hours_ones_layer]		= bitmap_layer_create(GRect(35, 95, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[hours_ones_layer]));
	
	dlayers[time_seperator_layer]	= bitmap_layer_create(GRect(70, 115, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[time_seperator_layer]));
	
	dlayers[mins_tens_layer]		= bitmap_layer_create(GRect(80, 95, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[mins_tens_layer]));
	
	dlayers[mins_ones_layer]		= bitmap_layer_create(GRect(115, 95, 144, 50));
	layer_add_child(window_layer, bitmap_layer_get_layer(dlayers[mins_ones_layer]));
	
	for (int i = 0; i < NUMBER_OF_IMAGES; i++)
	{
		images[i] = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[i]);
	}
	
	bitmap_layer_set_bitmap(dlayers[topbar_layer], images[42]);
	bitmap_layer_set_bitmap(dlayers[time_seperator_layer], images[12]);
	bitmap_layer_set_bitmap(dlayers[month_seperator_layer], images[41]);
	
	// init the face (place images etc)
	time_t unix_now = time(NULL);
	struct tm *t = localtime(&unix_now);
	handle_second_tick(t, 1);
	
	// start the tick subscription - every second update
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

void handle_deinit(void)
{
	tick_timer_service_unsubscribe();

	for (int i = 0; i < NUMBER_OF_IMAGES; i++)
	{
		gbitmap_destroy(images[i]);
	}

	bitmap_layer_destroy(dlayers[topbar_layer]);
	bitmap_layer_destroy(dlayers[month_tens_layer]);
	bitmap_layer_destroy(dlayers[month_ones_layer]);
	bitmap_layer_destroy(dlayers[month_seperator_layer]);
	bitmap_layer_destroy(dlayers[day_tens_layer]);
	bitmap_layer_destroy(dlayers[day_ones_layer]);
	bitmap_layer_destroy(dlayers[day_text_layer]);
	bitmap_layer_destroy(dlayers[secs_tens_layer]);
	bitmap_layer_destroy(dlayers[secs_ones_layer]);
	bitmap_layer_destroy(dlayers[ampm_layer]);
	bitmap_layer_destroy(dlayers[hours_tens_layer]);
	bitmap_layer_destroy(dlayers[hours_ones_layer]);
	bitmap_layer_destroy(dlayers[time_seperator_layer]);
	bitmap_layer_destroy(dlayers[mins_tens_layer]);
	bitmap_layer_destroy(dlayers[mins_ones_layer]);

	window_destroy(window);
}

int main(void)
{
	handle_init();
	app_event_loop();
	handle_deinit();
	return 1;
}
