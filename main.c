#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include "./Btree/inc/btree.h"

GtkWidget *textview1,*textview2,*textview3,*textview4;
GtkWidget *searchentry;
GtkWidget *entry1,*entry2;
GtkWidget *window,*window_them_tu,*window_sua_tu,*window_information;

GtkListStore *list;
GtkTreeIter Iter;

BTA *book;

char historytext[5000];
char suggetword[40];
int flag,sai=1;

void open_book();
void Show_message(GtkWidget * parent , GtkMessageType type,  char * mms, char * content);
void set_mean_textview_text(char * text);
void set_history_textview_text(char * text);

void click_search();
void on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void check_work(char *word1);
void find_work_mean(char *word);

void click_update();
void update();

void click_edit();
void edit();

void click_delete();
void click_information();




int main(int argc, char *argv[])
{
    GtkBuilder      *builder;
    GtkEntryCompletion *comple;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    
    // khoi tao bien tuong tac
    textview1 = GTK_WIDGET(gtk_builder_get_object(builder, "textview1"));
    textview2 = GTK_WIDGET(gtk_builder_get_object(builder, "textview2"));
    searchentry=GTK_WIDGET(gtk_builder_get_object(builder, "searchentry1"));

    // khoi taoj phan goi y tu
    comple = gtk_entry_completion_new();
    gtk_entry_completion_set_text_column(comple, 0);
    list = gtk_list_store_new(1, G_TYPE_STRING);
    gtk_entry_completion_set_model(comple, GTK_TREE_MODEL(list));
    gtk_entry_set_completion(GTK_ENTRY(searchentry), comple);

    g_signal_connect (searchentry, "key_press_event", G_CALLBACK (on_key_press), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    
    if(!btinit())
        open_book();
    g_object_unref(builder);

    gtk_widget_show(window);                
    gtk_main();

    btcls(book);
    return 0;
}


void open_book()
{
    book=btopn("AnhViet.dat",0,0);
}

void set_mean_textview_text(char * text) 
{
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
    if (buffer == NULL) {
        printf("Get buffer fail!");
        buffer = gtk_text_buffer_new(NULL);
    }
    gtk_text_buffer_set_text(buffer, text, -1);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview1), buffer);
}

void set_history_textview_text(char * text) 
{ 
    GtkTextBuffer *buffer;
    GtkTextIter st_iter;
    GtkTextIter ed_iter;  
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview2));
    gtk_text_buffer_set_text(buffer, text, -1);
    gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview2)), &st_iter);
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview2)), &ed_iter);
    char *histext =  gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview2)), &st_iter, &ed_iter, FALSE);
    gtk_text_buffer_set_text(buffer,histext, -1);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview2), buffer);
}

void click_search()
{
    char word[40];
    strcpy(word,gtk_entry_get_text(GTK_ENTRY(searchentry)));
    find_work_mean(word);
}

void find_work_mean(char *word)
{
    char mean[5000];
    int size;
    if(btsel(book,word,mean,5000,&size)){
           set_mean_textview_text("\nRất tiếc, từ này hiện không có trong từ điển."
                          "\n\nGợi ý:\t-Nhấn tab để tìm từ gần giống từ vừa nhập!"
                          "\n\t\t-Hoặc nhấn nút \"Thêm từ\", để bổ sung vào từ điển.");
        }
    else            
        {
            set_mean_textview_text(mean);
            strcat(historytext,word);
            strcat(historytext,"\n");
            set_history_textview_text(historytext);
        }
}
void on_key_press (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    char word[40];
    int x;
    strcpy(word, gtk_entry_get_text(GTK_ENTRY(searchentry)));
    x=strlen(word); if(x==0) sai=1;
    if (event->keyval != GDK_KEY_BackSpace)
    {

        if(event->keyval == GDK_KEY_Tab || event->keyval == GDK_KEY_KP_Enter)
        {
            gtk_entry_set_text(GTK_ENTRY(searchentry),suggetword);
            find_work_mean(suggetword);
        }
        else
        {
            word[x]=event->keyval;
            word[x+1]='\0';
            flag=0;
            gtk_list_store_clear(list);
            if(sai!=0)
                check_work(word);
        }   
    }
    else
    {
        if(x!=1)
        {
            word[x - 1] = '\0';
            if(word[0]!= '\0')
            flag=0;
            sai=1;
            gtk_list_store_clear(list);
            if(strlen(word) <= strlen(suggetword))
            check_work(word);  
        } 
    }
}

void check_work(char *word1)
{
    int dai1,size,i,j,first=0,k=0,q=0;
    char listword[40],word[40],nghia[5000];
    char kd[1];
    kd[0]=word1[0];
    kd[1]='\0';
    dai1=strlen(word1);
    flag=1;
    btsel(book,kd,nghia,sizeof(char*),&size);
        while(btseln(book,word,nghia,5000,&size)==0 )
            {

                i=0;
                j=0;
                if(flag==0) break;
                while(1)
                {
                    if(flag==0) break;
                    if(word1[0]<word[0])    q=1;   
                    if(word1[i]==word[i])
                        i++;
                    else
                        break;
                    if(i==dai1)
                    {
                        j=1;
                        if(first==0) strcpy(suggetword,word);
                        first++;
                        break;
                    }
                }
                if(flag==0 || q==1) break;
                sai=0;
                if(j==1)
                {
                    sai=1;
                    if(flag==0) break;
                    strcpy(listword,word);
                    gtk_list_store_append(list, &Iter);//them list moi
                    gtk_list_store_set(list, &Iter, 0, listword, -1 );
                    k++;
                    if(flag==0 || k == 15) break;
                }
            }
}

void Show_message(GtkWidget * parent , GtkMessageType type,  char * mms, char * content) 
{
    GtkWidget *mdialog;
    mdialog = gtk_message_dialog_new(GTK_WINDOW(parent),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     type,
                                     GTK_BUTTONS_OK,
                                     "%s", mms);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(mdialog), "%s",  content);
    gtk_dialog_run(GTK_DIALOG(mdialog));
    gtk_widget_destroy(mdialog);
}
void click_update()
{
    GtkBuilder      *builder;
    
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);
    
    window_them_tu = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    gtk_builder_connect_signals(builder, NULL);

    entry1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
    textview3 = GTK_WIDGET(gtk_builder_get_object(builder, "textview3"));

    g_object_unref(builder);
    gtk_widget_show(window_them_tu);
}

void update()
{
    GtkTextIter st_iter;
    GtkTextIter ed_iter;
    char word[40];
    strcpy(word,gtk_entry_get_text(GTK_ENTRY(entry1)));
    gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview3)), &st_iter);
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview3)), &ed_iter);
    char *nghia =  gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview3)), &st_iter, &ed_iter, FALSE);
    if(btins(book,word,nghia,strlen(nghia)+1)) 
    {
        Show_message(window_them_tu, GTK_MESSAGE_ERROR, "Xảy ra lỗi!", "Từ có thể đã tồn tại trong từ điển.");
    }
    else 
        Show_message(window_them_tu, GTK_MESSAGE_INFO, "Thành công !","Từ mới đã được thêm vào từ điển.");
    gtk_widget_destroy(window_them_tu);
}

void click_edit()
{
    GtkBuilder      *builder;
    char word[40];

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);
    
    window_sua_tu = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
    gtk_builder_connect_signals(builder, NULL);

    entry2 = GTK_WIDGET(gtk_builder_get_object(builder, "entry2"));
    textview4 = GTK_WIDGET(gtk_builder_get_object(builder, "textview4"));

    strcpy(word,gtk_entry_get_text(GTK_ENTRY(searchentry)));
    gtk_entry_set_text(GTK_ENTRY(entry2),word);
    
    g_object_unref(builder);
    gtk_widget_show(window_sua_tu); 
}

void edit()
{
    GtkTextIter st_iter;
    GtkTextIter ed_iter;
    char word[40];
    strcpy(word,gtk_entry_get_text(GTK_ENTRY(searchentry)));
    gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview4)), &st_iter);
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview4)), &ed_iter);
    char * nghia =  gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview4)), &st_iter, &ed_iter, FALSE);
    btupd(book,word,nghia,strlen(nghia)+1);
    Show_message(window_sua_tu, GTK_MESSAGE_INFO, "Thành công !", "Cập nhật từ thành công.");
    gtk_widget_destroy(window_sua_tu);
}

void click_delete()
{
    char word[40];
    strcpy(word,gtk_entry_get_text(GTK_ENTRY(searchentry)));
    btdel(book, word);
    Show_message(window, GTK_MESSAGE_ERROR, "Xóa từ", "Xóa từ thành công.");
}

void click_information()
{
	GtkBuilder      *builder;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);
    window_information = GTK_WIDGET(gtk_builder_get_object(builder, "window3"));
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_widget_show(window_information); 
} 
