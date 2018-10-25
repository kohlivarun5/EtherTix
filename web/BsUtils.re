type location;
[@bs.val] external location : location = "location";

type search = string;
[@bs.get] external search : location => search = "";

type origin = string;
[@bs.get] external origin : location => origin = "";

type pathname = string;
[@bs.get] external pathname : location => pathname = "";


[@bs.val] external decodeURIComponent : string => string = "";
[@bs.val] external encodeURIComponent : string => string = "";

let createSearchUri(key,value) = 
    location |> origin
    |> Js.String.concat(location |> pathname)
    |> Js.String.concatMany( [| "?", key, "=", value |] )

let getSearchValueByKey(key) = 
  location 
  |> search 
  |> Js.String.split("?")
  |> (fun (query) => switch(query) {
    | [||] | [|_|] => None
    | [|_,query|] => (
          query 
          |> Js.String.split("&") 
          |> Js.Array.map(Js.String.split("="))
          |> Js.Array.map((key_value) => 
              (decodeURIComponent(key_value[0]),decodeURIComponent(key_value[1])))
          |> Js.Array.find(((k,_)) => key == k)
          |> (fun (key_value) => switch(key_value) {
              | None => None 
              | Some((_,value)) => Some(value)
            })
      )
    | _ => assert(false)
  })

let createEventLinkUriComponent(~address,~description) =
  createSearchUri("event",address)
  |> Js.String.concat("\n")
  |> Js.String.concat("\n")
  |> Js.String.concat(description)
  |> encodeURIComponent
