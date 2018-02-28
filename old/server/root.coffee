hash = Math.random().toString()

module.exports = (app) ->
  app.get '/*', (req, res, next) ->
    res.send """
      <!DOCTYPE html>
      <html class="notouch">
        <head>
          <meta charset="utf-8">
          <meta http-equiv="X-UA-Compatible" content="IE=edge, chrome=1" />
          <meta name="viewport" content="width=device-width,user-scalable=no">
          <title>Fantoccini</title>
          <style>
            html, body {
              overflow: hidden;
              width: 100%;
              height: 100%;
              margin: 0;
              padding: 0;
              touch-action: none;
            }

            body > div {
              top: auto !important;
              bottom: 0 !important;
              left: auto !important;
              right: 0 !important;
            }
          </style>
        </head>
        <body>
          <script src="/dist/bundle.js?#{hash}"></script>
        </body>
      </html>
    """
