
/*
 * Copyright (C) Liu Lantao
 */


#include <ngx_http.h>

#include "ngx_traffic_accounting.h"
#include "ngx_http_accounting_module.h"


static ngx_int_t ngx_http_accounting_init(ngx_conf_t *cf);

static ngx_int_t ngx_http_accounting_process_init(ngx_cycle_t *cycle);
static void ngx_http_accounting_process_exit(ngx_cycle_t *cycle);

static char *ngx_http_accounting_set_accounting_id(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t  ngx_http_accounting_commands[] = {
    { ngx_string("http_accounting"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_flag_slot,
      NGX_HTTP_MAIN_CONF_OFFSET,
      offsetof(ngx_http_accounting_main_conf_t, enable),
      NULL},

    { ngx_string("http_accounting_id"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_http_accounting_set_accounting_id,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL},

    { ngx_string("http_accounting_interval"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_sec_slot,
      NGX_HTTP_MAIN_CONF_OFFSET,
      offsetof(ngx_http_accounting_main_conf_t, interval),
      NULL},

    { ngx_string("http_accounting_perturb"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_flag_slot,
      NGX_HTTP_MAIN_CONF_OFFSET,
      offsetof(ngx_http_accounting_main_conf_t, perturb),
      NULL},

    { ngx_string("http_accounting_log"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_1MORE,
      ngx_traffic_accounting_set_log,
      NGX_HTTP_MAIN_CONF_OFFSET,
      0,
      NULL},

    ngx_null_command
};


static ngx_http_module_t  ngx_http_accounting_ctx = {
    NULL,                                   /* preconfiguration */
    ngx_http_accounting_init,               /* postconfiguration */
    ngx_traffic_accounting_create_main_conf,/* create main configuration */
    ngx_traffic_accounting_init_main_conf,  /* init main configuration */
    NULL,                                   /* create server configuration */
    NULL,                                   /* merge server configuration */
    ngx_traffic_accounting_create_loc_conf, /* create location configuration */
    ngx_traffic_accounting_merge_loc_conf   /* merge location configuration */
};


ngx_module_t ngx_http_accounting_module = {
    NGX_MODULE_V1,
    &ngx_http_accounting_ctx,               /* module context */
    ngx_http_accounting_commands,           /* module directives */
    NGX_HTTP_MODULE,                        /* module type */
    NULL,                                   /* init master */
    NULL,                                   /* init module */
    ngx_http_accounting_process_init,       /* init process */
    NULL,                                   /* init thread */
    NULL,                                   /* exit thread */
    ngx_http_accounting_process_exit,       /* exit process */
    NULL,                                   /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_int_t
ngx_http_accounting_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt              *h;
    ngx_http_core_main_conf_t        *cmcf;
    ngx_http_accounting_main_conf_t  *amcf;

    amcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_accounting_module);
    if (!amcf->enable) {
        return NGX_OK;
    }

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_LOG_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_accounting_handler;

    return NGX_OK;
}


static ngx_int_t
ngx_http_accounting_process_init(ngx_cycle_t *cycle)
{
    return ngx_http_accounting_worker_process_init(cycle);
}


static void
ngx_http_accounting_process_exit(ngx_cycle_t *cycle)
{
	return ngx_http_accounting_worker_process_exit(cycle);
}


static char *
ngx_http_accounting_set_accounting_id(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    return ngx_traffic_accounting_set_accounting_id(cf, cmd, conf, ngx_http_get_variable_index);
}
