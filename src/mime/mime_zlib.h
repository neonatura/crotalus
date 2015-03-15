
int mime_head_gzip(request *req);

int mime_get_gzip(request *req);

int mime_filter_init_deflate(request *req);
int mime_filter_write_deflate(request *req);
int mime_filter_flush_deflate(request *req);
int mime_filter_init_gzip(request *req);
int mime_filter_write_gzip(request *req);
int mime_filter_flush_gzip(request *req);
