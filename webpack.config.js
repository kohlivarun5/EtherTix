const path = require('path');
const webpack = require('webpack');
const outputDir = path.join(__dirname, "build/");

const isProd = process.env.NODE_ENV === 'production';

module.exports = {
  entry: './web/Index.bs.js',
  mode: isProd ? 'production' : 'development',
  output: {
    path: outputDir,
    publicPath: outputDir,
    filename: 'Index.js',
  },
  node: {
    fs: 'empty'
  },
  resolve: {
    modules: [
      path.resolve(__dirname, './node_modules'),
      path.resolve(__dirname, './web'),
      path.resolve(__dirname, './sol')
    ],
    extensions: ['.js', '.json']
  },
};
