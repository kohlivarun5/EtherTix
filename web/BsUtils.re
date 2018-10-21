type location;
[@bs.val] external location : location = "location";

type search = string;
[@bs.get] external search : location => search = "";

[@bs.val] external decodeURIComponent : string => string = "";

let getSearchValueByKey(key) = 
  location 
  |> search 
  |> Js.String.split("?")
  |> (fun (query) => switch(query) {
    | [||] | [|_|] => None
    | [|uri,query|] => (
          query 
          |> Js.String.split("&") 
          |> Js.Array.map(Js.String.split("="))
          |> Js.Array.map((key_value) => 
              (decodeURIComponent(key_value[0]),decodeURIComponent(key_value[1])))
          |> Js.Array.find(((k,value)) => key == k)
          |> (fun (key_value) => switch(key_value) {
              | None => None 
              | Some((_,value)) => Some(value)
            })
      )
  })
