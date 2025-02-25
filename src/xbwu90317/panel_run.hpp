#include <unistd.h>  // for write
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>

using boost::asio::ip::tcp;

extern boost::asio::io_context io_context;

const char *panel_body = R"PANEL(
<!DOCTYPE html>
<html lang="en">
  <head>
    <title>NP Project 3 Panel</title>
    <link
      rel="stylesheet"
      href="https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css"
      integrity="sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2"
      crossorigin="anonymous"
    />
    <link
      href="https://fonts.googleapis.com/css?family=Source+Code+Pro"
      rel="stylesheet"
    />
    <link
      rel="icon"
      type="image/png"
      href="https://cdn4.iconfinder.com/data/icons/iconsimple-setting-time/512/dashboard-512.png"
    />
    <style>
      * {
        font-family: 'Source Code Pro', monospace;
      }
    </style>
  </head>
  <body class="bg-secondary pt-5">
    <form action="console.cgi" method="GET">
      <table class="table mx-auto bg-light" style="width: inherit">
        <thead class="thead-dark">
          <tr>
            <th scope="col">#</th>
            <th scope="col">Host</th>
            <th scope="col">Port</th>
            <th scope="col">Input File</th>
          </tr>
        </thead>
        <tbody>
          <tr>
            <th scope="row" class="align-middle">Session 1</th>
            <td>
              <div class="input-group">
                <select name="h0" class="custom-select">
                  <option></option><option value="nplinux1.cs.nycu.edu.tw">nplinux1</option><option value="nplinux2.cs.nycu.edu.tw">nplinux2</option><option value="nplinux3.cs.nycu.edu.tw">nplinux3</option><option value="nplinux4.cs.nycu.edu.tw">nplinux4</option><option value="nplinux5.cs.nycu.edu.tw">nplinux5</option><option value="nplinux6.cs.nycu.edu.tw">nplinux6</option><option value="nplinux7.cs.nycu.edu.tw">nplinux7</option><option value="nplinux8.cs.nycu.edu.tw">nplinux8</option><option value="nplinux9.cs.nycu.edu.tw">nplinux9</option><option value="nplinux10.cs.nycu.edu.tw">nplinux10</option><option value="nplinux11.cs.nycu.edu.tw">nplinux11</option><option value="nplinux12.cs.nycu.edu.tw">nplinux12</option>
                </select>
                <div class="input-group-append">
                  <span class="input-group-text">.cs.nycu.edu.tw</span>
                </div>
              </div>
            </td>
            <td>
              <input name="p0" type="text" class="form-control" size="5" />
            </td>
            <td>
              <select name="f0" class="custom-select">
                <option></option>
                <option value="t1.txt">t1.txt</option><option value="t2.txt">t2.txt</option><option value="t3.txt">t3.txt</option><option value="t4.txt">t4.txt</option><option value="t5.txt">t5.txt</option>
              </select>
            </td>
          </tr>
          <tr>
            <th scope="row" class="align-middle">Session 2</th>
            <td>
              <div class="input-group">
                <select name="h1" class="custom-select">
                  <option></option><option value="nplinux1.cs.nycu.edu.tw">nplinux1</option><option value="nplinux2.cs.nycu.edu.tw">nplinux2</option><option value="nplinux3.cs.nycu.edu.tw">nplinux3</option><option value="nplinux4.cs.nycu.edu.tw">nplinux4</option><option value="nplinux5.cs.nycu.edu.tw">nplinux5</option><option value="nplinux6.cs.nycu.edu.tw">nplinux6</option><option value="nplinux7.cs.nycu.edu.tw">nplinux7</option><option value="nplinux8.cs.nycu.edu.tw">nplinux8</option><option value="nplinux9.cs.nycu.edu.tw">nplinux9</option><option value="nplinux10.cs.nycu.edu.tw">nplinux10</option><option value="nplinux11.cs.nycu.edu.tw">nplinux11</option><option value="nplinux12.cs.nycu.edu.tw">nplinux12</option>
                </select>
                <div class="input-group-append">
                  <span class="input-group-text">.cs.nycu.edu.tw</span>
                </div>
              </div>
            </td>
            <td>
              <input name="p1" type="text" class="form-control" size="5" />
            </td>
            <td>
              <select name="f1" class="custom-select">
                <option></option>
                <option value="t1.txt">t1.txt</option><option value="t2.txt">t2.txt</option><option value="t3.txt">t3.txt</option><option value="t4.txt">t4.txt</option><option value="t5.txt">t5.txt</option>
              </select>
            </td>
          </tr>
          <tr>
            <th scope="row" class="align-middle">Session 3</th>
            <td>
              <div class="input-group">
                <select name="h2" class="custom-select">
                  <option></option><option value="nplinux1.cs.nycu.edu.tw">nplinux1</option><option value="nplinux2.cs.nycu.edu.tw">nplinux2</option><option value="nplinux3.cs.nycu.edu.tw">nplinux3</option><option value="nplinux4.cs.nycu.edu.tw">nplinux4</option><option value="nplinux5.cs.nycu.edu.tw">nplinux5</option><option value="nplinux6.cs.nycu.edu.tw">nplinux6</option><option value="nplinux7.cs.nycu.edu.tw">nplinux7</option><option value="nplinux8.cs.nycu.edu.tw">nplinux8</option><option value="nplinux9.cs.nycu.edu.tw">nplinux9</option><option value="nplinux10.cs.nycu.edu.tw">nplinux10</option><option value="nplinux11.cs.nycu.edu.tw">nplinux11</option><option value="nplinux12.cs.nycu.edu.tw">nplinux12</option>
                </select>
                <div class="input-group-append">
                  <span class="input-group-text">.cs.nycu.edu.tw</span>
                </div>
              </div>
            </td>
            <td>
              <input name="p2" type="text" class="form-control" size="5" />
            </td>
            <td>
              <select name="f2" class="custom-select">
                <option></option>
                <option value="t1.txt">t1.txt</option><option value="t2.txt">t2.txt</option><option value="t3.txt">t3.txt</option><option value="t4.txt">t4.txt</option><option value="t5.txt">t5.txt</option>
              </select>
            </td>
          </tr>
          <tr>
            <th scope="row" class="align-middle">Session 4</th>
            <td>
              <div class="input-group">
                <select name="h3" class="custom-select">
                  <option></option><option value="nplinux1.cs.nycu.edu.tw">nplinux1</option><option value="nplinux2.cs.nycu.edu.tw">nplinux2</option><option value="nplinux3.cs.nycu.edu.tw">nplinux3</option><option value="nplinux4.cs.nycu.edu.tw">nplinux4</option><option value="nplinux5.cs.nycu.edu.tw">nplinux5</option><option value="nplinux6.cs.nycu.edu.tw">nplinux6</option><option value="nplinux7.cs.nycu.edu.tw">nplinux7</option><option value="nplinux8.cs.nycu.edu.tw">nplinux8</option><option value="nplinux9.cs.nycu.edu.tw">nplinux9</option><option value="nplinux10.cs.nycu.edu.tw">nplinux10</option><option value="nplinux11.cs.nycu.edu.tw">nplinux11</option><option value="nplinux12.cs.nycu.edu.tw">nplinux12</option>
                </select>
                <div class="input-group-append">
                  <span class="input-group-text">.cs.nycu.edu.tw</span>
                </div>
              </div>
            </td>
            <td>
              <input name="p3" type="text" class="form-control" size="5" />
            </td>
            <td>
              <select name="f3" class="custom-select">
                <option></option>
                <option value="t1.txt">t1.txt</option><option value="t2.txt">t2.txt</option><option value="t3.txt">t3.txt</option><option value="t4.txt">t4.txt</option><option value="t5.txt">t5.txt</option>
              </select>
            </td>
          </tr>
          <tr>
            <th scope="row" class="align-middle">Session 5</th>
            <td>
              <div class="input-group">
                <select name="h4" class="custom-select">
                  <option></option><option value="nplinux1.cs.nycu.edu.tw">nplinux1</option><option value="nplinux2.cs.nycu.edu.tw">nplinux2</option><option value="nplinux3.cs.nycu.edu.tw">nplinux3</option><option value="nplinux4.cs.nycu.edu.tw">nplinux4</option><option value="nplinux5.cs.nycu.edu.tw">nplinux5</option><option value="nplinux6.cs.nycu.edu.tw">nplinux6</option><option value="nplinux7.cs.nycu.edu.tw">nplinux7</option><option value="nplinux8.cs.nycu.edu.tw">nplinux8</option><option value="nplinux9.cs.nycu.edu.tw">nplinux9</option><option value="nplinux10.cs.nycu.edu.tw">nplinux10</option><option value="nplinux11.cs.nycu.edu.tw">nplinux11</option><option value="nplinux12.cs.nycu.edu.tw">nplinux12</option>
                </select>
                <div class="input-group-append">
                  <span class="input-group-text">.cs.nycu.edu.tw</span>
                </div>
              </div>
            </td>
            <td>
              <input name="p4" type="text" class="form-control" size="5" />
            </td>
            <td>
              <select name="f4" class="custom-select">
                <option></option>
                <option value="t1.txt">t1.txt</option><option value="t2.txt">t2.txt</option><option value="t3.txt">t3.txt</option><option value="t4.txt">t4.txt</option><option value="t5.txt">t5.txt</option>
              </select>
            </td>
          </tr>
          <tr>
            <td colspan="3"></td>
            <td>
              <button type="submit" class="btn btn-info btn-block">Run</button>
            </td>
          </tr>
        </tbody>
      </table>
    </form>
  </body>
</html>
)PANEL";

void panel_run(std::shared_ptr<tcp::socket> http)
{
    // Print HTTP headers
    boost::asio::streambuf buf;
    std::ostream bout(&buf);
    bout << "Content-type: text/html\r\n\r\n";
    bout << panel_body;
    boost::asio::write(*http.get(), buf);
}
