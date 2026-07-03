#include "Extended_BPLib.h"

bool UExtendedVarsBPLibrary::StringToJsonObjectArray(TArray<TSharedPtr<FJsonValue>>& OutArray, const FString& InJsonArrayString)
{
    OutArray.Empty();

    const FString JsonText = InJsonArrayString.TrimStartAndEnd();

    if (!JsonText.StartsWith(TEXT("[")) || !JsonText.EndsWith(TEXT("]")))
    {
        return false;
    }

    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);

    if (!FJsonSerializer::Deserialize(Reader, OutArray))
    {
        return false;
    }

    return true;
}

FJsonObjectWrapper UExtendedVarsBPLibrary::MapToJson(const TMap<FString, FString>& In_Map)
{
    FJsonObjectWrapper ResultJson;

    if (In_Map.IsEmpty())
    {
        return ResultJson;
    }

    for (const TPair<FString, FString>& EachPair : In_Map)
    {
        ResultJson.JsonObject->SetStringField(EachPair.Key, EachPair.Value);
    }

    return ResultJson;
}

FString UExtendedVarsBPLibrary::BeautifyJson(FString In_Json)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(In_Json);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        return FString();
    }

    FString BeautifiedJson;
    TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&BeautifiedJson);

    return FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer) ? BeautifiedJson : FString();
}

FString UExtendedVarsBPLibrary::PrintJsonObjectArray(TArray<FJsonObjectWrapper> In_Objects)
{
    TArray<TSharedPtr<FJsonValue>> JsonArray;

    for (const FJsonObjectWrapper EachJson : In_Objects)
    {
        JsonArray.Add(MakeShared<FJsonValueObject>(EachJson.JsonObject));
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonArray, Writer);

    return OutputString;
}

FJsonObjectWrapper UExtendedVarsBPLibrary::GetElementFromJsonObjectArray(TArray<FJsonObjectWrapper> In_Objects, int32 Index)
{
    return In_Objects.IsValidIndex(Index) ? In_Objects[Index] : FJsonObjectWrapper();
}

TArray<FJsonObjectWrapper> UExtendedVarsBPLibrary::GetObjectArrayFieldFromString(const FString& JsonString, const FString& FieldName)
{
    TArray<FJsonObjectWrapper> OutArray;
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        const TArray<TSharedPtr<FJsonValue>>* ResultField;

        if (JsonObject->TryGetArrayField(FieldName, ResultField))
        {
            for (const TSharedPtr<FJsonValue>& Value : *ResultField)
            {
                TSharedPtr<FJsonObject> Each_Object = Value->AsObject();

                if (Each_Object.IsValid())
                {
                    FJsonObjectWrapper Each_Wrapper;
                    Each_Wrapper.JsonObject = Each_Object;
                    OutArray.Add(Each_Wrapper);
                }
            }
        }
    }

    return OutArray;
}

TArray<FJsonObjectWrapper> UExtendedVarsBPLibrary::GetObjectArrayFieldFromObject(FJsonObjectWrapper In_Object, const FString& FieldName)
{
    TArray<FJsonObjectWrapper> OutArray;
    const TArray<TSharedPtr<FJsonValue>>* ResultField;

    if (In_Object.JsonObject->TryGetArrayField(FieldName, ResultField))
    {
        for (const TSharedPtr<FJsonValue>& Value : *ResultField)
        {
            TSharedPtr<FJsonObject> Each_Object = Value->AsObject();

            if (Each_Object.IsValid())
            {
                FJsonObjectWrapper Each_Wrapper;
                Each_Wrapper.JsonObject = Each_Object;
                OutArray.Add(Each_Wrapper);
            }
        }
    }

    return OutArray;
}

TArray<FString> UExtendedVarsBPLibrary::GetValueArrayFieldFromString(const FString& JsonString, const FString& FieldName)
{
    TArray<FString> OutArray;
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        const TArray<TSharedPtr<FJsonValue>>* ResultField;

        if (JsonObject->TryGetArrayField(FieldName, ResultField))
        {
            for (const TSharedPtr<FJsonValue>& Value : *ResultField)
            {
                OutArray.Add(Value->AsString());
            }
        }
    }

    return OutArray;
}

TArray<FString> UExtendedVarsBPLibrary::GetValueArrayFieldFromObject(FJsonObjectWrapper In_Object, const FString& FieldName)
{
    TArray<FString> OutArray;
    const TArray<TSharedPtr<FJsonValue>>* ResultField;

    if (In_Object.JsonObject->TryGetArrayField(FieldName, ResultField))
    {
        for (const TSharedPtr<FJsonValue>& Value : *ResultField)
        {
            OutArray.Add(Value->AsString());
        }
    }

    return OutArray;
}