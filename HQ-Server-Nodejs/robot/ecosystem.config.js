module.exports = {
  /**
   * Application configuration section
   * http://pm2.keymetrics.io/docs/usage/application-declaration/
   */
  apps : [

    // First application
    {
      name      : 'HQ-Robot',
      script    : 'test.js',
      args      : '319294c67d174c878cc7922551e6e773 12345678 agora_hq_cc_server_en_2',
      env: {
        COMMON_VARIABLE: 'true'
      },
      env_production : {
        NODE_ENV: 'production'
      }
    }
  ]
};
